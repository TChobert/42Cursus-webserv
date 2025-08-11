#include "CGIHandler.hpp"
#include "Executor.hpp"
#include "webserv_utils.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>
#include <cstdio>

//ATTENTION: ce qui peut etre encore a faire, a checker..
//- Si CGI ecrit par un tiers, attention aux headers dangereux (ex : Transfer-Encoding, Content-Length, etc.)
//- Redirections (status 3xx et Location): si CGI donne Location, peut-etre devoir le gerer dans updateResponseData?
//- gestion timeouts



/* ---------------- PRIVATE METHODS ------------------ */

//Exemple:
// line = "Content-Type: text/html"

// colon = 12

// key = line.substr(0, 12) = "Content-Type"
// value = line.substr(13) = " text/html"
// value.erase(0, value.find_first_not_of(" \t")) => "text/html"

void	CGIHandler::parseCgiOutput(Conversation& conv)
{
	const std::string& raw = conv.cgiOutput;

	std::cout << "[parseCgiOutput] CGI raw output size: " << raw.size() << std::endl;

	size_t headerEnd = raw.find("\r\n\r\n");
	if (headerEnd == std::string::npos) //respecte pas format
	{
		std::cerr << "[parseCgiOutput] ERROR: CGI output does not contain header/body separator." << std::endl;
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	}

	std::string headersPart = raw.substr(0, headerEnd);
	std::string bodyPart = raw.substr(headerEnd + 4); // skip \r\n\r\n

	std::cout << "[parseCgiOutput] Headers part size: " << headersPart.size() << std::endl;
	std::cout << "[parseCgiOutput] Body part size: " << bodyPart.size() << std::endl;

	std::istringstream stream(headersPart);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.empty())
			continue;
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1); // nettoie ligne en enlevant le "\r" s'il y en a un

		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue; // ligne malformee, on continue

		//separation "key: valeur"
		std::string key = trim(line.substr(0, colon));
		std::string value = trim(line.substr(colon + 1));

		//repartition des headers dans leurs variables respectives
		std::string keyLower = key;
		toLower(keyLower);

		std::cout << "[parseCgiOutput] Header: \"" << key << "\" => \"" << value << "\"" << std::endl;

		if (keyLower == "content-type")
			conv.resp.contentType = value;
		else if (keyLower == "status")
		{
			int status = atoi(value.c_str());
			if (status <= 0 || status > 599)
				status = INTERNAL_SERVER_ERROR;
			conv.resp.status = static_cast<statusCode>(status);
		}
		else if (keyLower == "set-cookie")
			conv.resp.setCookies.push_back(value);
		else
			conv.resp.header[key] = value;
	}

	conv.resp.body = bodyPart;

	if (conv.resp.status == 0)
	{
		conv.resp.status = OK;
		std::cout << "[parseCgiOutput] No status header found, defaulting to 200 OK." << std::endl;
	}
}


char**	CGIHandler::prepareEnv(Conversation& conv)
{
	std::vector<std::string> envStrings;

	//attention: difference entre SCRIPT_FILENAME et PATH_INFO

	//SCRIPT_FILENAME = chemin absolu complet vers le fichier script donc pathOnDisk
	//PATH_INFO = partie optionnelle de URL qui suit le script (parfois absent) > donne infos supp au moment de l'exec

	//Exemple:
	//URL = http://exemple.com/cgi-bin/monscript.py/foo/bar
	//SCRIPT_FILENAME = /var/www/cgi-bin/monscript.py
	//PATH_INFO = /foo/bar

	envStrings.push_back("REQUEST_METHOD=" + conv.req.method);
	envStrings.push_back("SCRIPT_FILENAME=" + conv.req.pathOnDisk);
	envStrings.push_back("QUERY_STRING=" + conv.req.query);
	envStrings.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envStrings.push_back("SERVER_NAME=" + conv.config.identity.serverName);
	envStrings.push_back("SERVER_PORT=" + intToString(conv.config.identity.port));

	if (conv.req.method == "POST")
	{
		envStrings.push_back("CONTENT_LENGTH=" + intToString(conv.req.body.size()));
		std::map<std::string, std::string>::const_iterator it = conv.req.header.find("Content-Type");
		if (it != conv.req.header.end())
			envStrings.push_back("CONTENT_TYPE=" + it->second);
	}

	std::cerr << "[prepareEnv] Environment variables prepared:" << std::endl;
	for (size_t i = 0; i < envStrings.size(); ++i)
	{
		std::cerr << "  " << envStrings[i] << std::endl;
	}

	char** envp = (char**)malloc(sizeof(char*) * (envStrings.size() + 1));
	if (!envp)
		return NULL;
	for (size_t i = 0; i < envStrings.size(); ++i)
	{
		envp[i] = strdup(envStrings[i].c_str());
		if (!envp[i])
		{
			std::cerr << "[prepareEnv] ERROR: strdup failed at index " << i << std::endl;
			freeEnv(envp);
			return NULL;
		}
	}
	envp[envStrings.size()] = NULL;
	return envp;
}

void	CGIHandler::handleGetCGI(Conversation& conv)
{
	//chemin absolu du script CGI a executer
	const std::string& scriptPath = conv.req.pathOnDisk;
	std::cout << "SCRIPT PATH HANDLEGETCGI = " << scriptPath << std::endl;

	//recuperer la bonne extension du script a faire (.py, .php, etc)
	std::string extension = getFileExtension(scriptPath);

	//recuperer le bon interpreteur par rapport a extension, exemple, .py >> python3
	std::map<cgiExtension, cgiHandler>::const_iterator it = conv.location->cgiHandlers.find(extension);
	if (it == conv.location->cgiHandlers.end())
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	const std::string& interpreter = it->second;
	std::cout << "INTERPRETER = " << interpreter << std::endl;

	//creation pipe pour que Webserv puisse lire la sortie du script (stockee dans le pipe)
	int pipe_out[2];
	if (pipe(pipe_out) == -1)
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);

	std::cout << "HELLO" << std::endl;

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cout << "PID < 0" << std::endl;
		close(pipe_out[0]);
		close(pipe_out[1]);
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	//process enfant: rediriger sortie stdout vers pipe
	if (pid == 0)
	{
		close(pipe_out[0]);
		if (dup2(pipe_out[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE); //kill processus enfant > waitpid parent avec erreur 500
		close(pipe_out[1]);

		//ICI - boucle de fermeture de fds herites du parent ?
		for (int fd = 3; fd < 1024; ++fd)
		{
			if (fd != STDOUT_FILENO)
				close(fd);
		}

		char** envp = prepareEnv(conv);
		if (!envp)
			exit(EXIT_FAILURE);
		for (int i = 0; envp[i]; ++i)
    		std::cerr << "envp[" << i << "] = " << envp[i] << std::endl;

		std::cerr << "[CHILD] PID: " << getpid() << " executing CGI" << std::endl;
		//{interpreter, scriptPath, NULL}
		char* argv[] = {const_cast<char*>(interpreter.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
		std::cerr << "[CHILD] launching execve: "
          << interpreter << " " << scriptPath << std::endl;
		execve(interpreter.c_str(), argv, envp);
		// {
    	// 		perror("execve failed");
   		// 		_exit(EXIT_FAILURE);
		// }
		freeEnv(envp);
		exit(EXIT_FAILURE);
	}

	//process parent: lire dans le pipe ce qui a ete stocke par execve
	//read > passage par epoll obligatoire
	if (pid > 0)
	{
		close(pipe_out[1]);
		conv.cgiPid = pid;
		conv.cgiStartTime = time(NULL);

		conv.tempFd = pipe_out[0]; //on veut lire ce qui a ete stocke dans le pipe
		int flags = fcntl(conv.tempFd, F_GETFL, 0);
		if (flags == -1) {
			perror("fcntl F_GETFL");
		}
		if (fcntl(conv.tempFd, F_SETFL, flags | O_NONBLOCK) == -1) {
			perror("fcntl F_SETFL O_NONBLOCK");
		}
		conv.eState = READ_EXEC_GET_CGI;
		conv.state = READ_EXEC;
		std::cerr << "END OF PROCESS" << std::endl;
	}
}

void	CGIHandler::handlePostCGI(Conversation& conv)
{
	const std::string& scriptPath = conv.req.pathOnDisk;
	std::string extension = getFileExtension(scriptPath);

	std::map<cgiExtension, cgiHandler>::const_iterator it = conv.location->cgiHandlers.find(extension);
	if (it == conv.location->cgiHandlers.end())
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	const std::string& interpreter = it->second;

	int pipe_in[2], pipe_out[2];
	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);

	pid_t pid = fork();
	if (pid < 0)
	{
		close(pipe_in[0]); close(pipe_in[1]);
		close(pipe_out[0]); close(pipe_out[1]);
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	}
	if (pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_in[1]); close(pipe_out[0]);
		close(pipe_in[0]); close(pipe_out[1]);

		//ICI - boucle de fermeture de fds herites du parent ?
		for (int fd = 3; fd < 1024; ++fd)
			close(fd);

		char** envp = prepareEnv(conv);
		if (!envp)
			exit(EXIT_FAILURE);
		char* argv[] = {const_cast<char*>(interpreter.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
		execve(interpreter.c_str(), argv, envp);
		freeEnv(envp);
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
	{
		close(pipe_in[0]);
		close(pipe_out[1]);
		conv.cgiPid = pid;
		conv.cgiStartTime = time(NULL);
		conv.cgiIn = pipe_in[1]; //ecrire dans CGI
		conv.cgiOut = pipe_out[0]; //lire sortie CGI

		const size_t chunkSize = 4096; //correspond a la taille dans Resume

		if (conv.req.body.size() <= chunkSize)
			conv.streamState = NORMAL;
		else
			conv.streamState = START_STREAM;

	//	conv.eState = WRITE_EXEC_POST_CGI;
		conv.state = CGI_EXECUTOR;
	}
}

/* ---------------- PUBLIC METHODS ------------------ */

bool CGIHandler::isCGI(const Conversation& conv)
{
	const std::string& path = conv.req.pathOnDisk;

	//Trouver l'extension
	std::size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos)
		return false; //Aucun point = aucune extension = pas CGI

	//Extraire l'extension
	std::string extension = path.substr(dotPos);

	//Recup la map des CGI handlers
	const std::map<cgiExtension, cgiHandler>& cgiMap = conv.location->cgiHandlers;

	//Verif extension
	bool isCgiExtension = (cgiMap.find(extension) != cgiMap.end());

	if (isCgiExtension)
		return true;
	else
		return false;
}

void CGIHandler::handleCGI(Conversation& conv)
{
	const std::string& method = conv.req.method;

	if (method == "GET")
		handleGetCGI(conv);
	else if (method == "POST")
		handlePostCGI(conv);
	else
		Executor::setResponse(conv, NOT_IMPLEMENTED);
}
