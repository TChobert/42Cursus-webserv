#include "CgiExecutor.hpp"
#include "Executor.hpp"
#include "webserv.hpp"
#include "webserv_utils.hpp"
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <sys/wait.h>
#include <cstdio>
#include <sstream>

CgiExecutor::CgiExecutor(int& epollFd) : _epollFd(epollFd) {}

CgiExecutor::~CgiExecutor(void) {}


// SETUP PROCESS ////////////////////////////////////////////////////////////

char**	CgiExecutor::prepareEnv(Conversation& conv)
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

void CgiExecutor::setupCgiProcessGet(Conversation& conv)
{
	const std::string& scriptPath = conv.req.pathOnDisk;
	std::cout << "SCRIPT PATH HANDLEGETCGI = " << scriptPath << std::endl;

	std::string extension = getFileExtension(scriptPath);

	std::map<cgiExtension, cgiHandler>::const_iterator it = conv.location->cgiHandlers.find(extension);
	if (it == conv.location->cgiHandlers.end())
		return Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
	const std::string& interpreter = it->second;

	int pipe_out[2];
	if (pipe(pipe_out) == -1)
		return Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cout << "PID < 0" << std::endl;
		close(pipe_out[0]);
		close(pipe_out[1]);
		Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
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

		std::cerr << "[CHILD] PID: " << getpid() << " executing CGI" << std::endl;
		//{interpreter, scriptPath, NULL}
		char* argv[] = {const_cast<char*>(interpreter.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
		execve(interpreter.c_str(), argv, envp);
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

		conv.cgiOut = pipe_out[0]; //on veut lire ce qui a ete stocke dans le pipe
		int flags = fcntl(conv.cgiOut, F_GETFL, 0);
		if (flags == -1) {
			perror("fcntl F_GETFL");
		}
		if (fcntl(conv.cgiOut, F_SETFL, flags | O_NONBLOCK) == -1) {
			perror("fcntl F_SETFL O_NONBLOCK");
		}
	}
}
void CgiExecutor::addCgiFdToEpoll(int fd, Conversation& conv)
{
	if (fd < 0)
		return;

	struct epoll_event ev;
	ev.data.ptr = &conv; // pointer vers la conversation
	ev.events = EPOLLIN;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
	{
		std::cerr << "Failed to add CGI fd to epoll: " << strerror(errno) << std::endl;
		close(fd);
	}
}

// RESPONSE + PARSE + ENVOI ///////////////////////////////////////////////////

void CgiExecutor::parseCgiHeaders(Conversation& conv, const std::string& headerPart)
{
	size_t start = 0;
	size_t end = 0;

	while (start < headerPart.size())
	{
		end = headerPart.find("\r\n", start);
		if (end == std::string::npos)
			end = headerPart.size();

		std::string line = headerPart.substr(start, end - start);
		start = end + 2;

		if (line.empty())
			continue;

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue;

		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
				value.erase(0, 1);
		while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == '\t'))
				value.erase(value.size() - 1, 1);

		conv.cgiHeaders[key] = value;
	}
}

void CgiExecutor::sendHeadersToClient(Conversation& conv)
{
	std::string response = "HTTP/1.1 200 OK\r\n";

	// Fusion validator headers + CGI headers
	std::map<std::string, std::string> allHeaders = conv.resp.header;
	for (std::map<std::string, std::string>::iterator it = conv.cgiHeaders.begin(); it != conv.cgiHeaders.end(); ++it)
		allHeaders[it->first] = it->second;

		// Construction du texte final
	for (std::map<std::string, std::string>::iterator it = allHeaders.begin(); it != allHeaders.end(); ++it)
		response += it->first + ": " + it->second + "\r\n";

	// Chunked si pas de Content-Length
	if (allHeaders.find("Content-Length") == allHeaders.end())
		response += "Transfer-Encoding: chunked\r\n";

	response += "\r\n"; // fin headers

	write(conv.fd, response.c_str(), response.size());
	conv.headersSent = true;
}

void CgiExecutor::sendBodyPartToClient(Conversation& conv, const std::string& body, size_t len)
{
	if (len == 0)
		return;

	// Si pas de Content-Length, on envoie en chunked
	bool useChunked = (conv.cgiHeaders.find("Content-Length") == conv.cgiHeaders.end());

	std::string out;
	if (useChunked)
	{
		// Format chunked : taille en hex + CRLF + data + CRLF
		std::ostringstream oss;
		oss << std::hex << len << "\r\n";  // convertit len en hexadécimal
		out += oss.str();
		out += body.substr(0, len);
		out += "\r\n";
	}
	else
	{
		out += body.substr(0, len);
	}

	// Envoi sur le socket client
	ssize_t sent = write(conv.fd, out.c_str(), out.size());
	if (sent < 0)
	{
		Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	// MAJ
	conv.bytesSent += sent;
}


// EXECUTION CGI //////////////////////////////////////////////////////////////

void CgiExecutor::executeCgiGet(Conversation& conv)
{
	if (conv.cgiOut < 0)
		return;

	char buffer[4096];
	ssize_t bytesRead = read(conv.cgiOut, buffer, sizeof(buffer));

	if (bytesRead < 0)
	{
		close(conv.cgiOut);
		conv.cgiOut = -1;
		Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
		return;
	}
	else if (bytesRead == 0)
	{
		int status;
		waitpid(conv.cgiPid, &status, 0);
		close(conv.cgiOut);
		conv.cgiOut = -1;

		if (!conv.headersSent)
			Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
		else
		{
			if (conv.cgiHeaders.find("Content-Length") == conv.cgiHeaders.end())
				write(conv.fd, "0\r\n\r\n", 5);

			conv.state = WRITE_CLIENT;
		}
		return;
	}
	if (!conv.headersSent)
	{
		conv.cgiOutput.append(buffer, bytesRead);

		size_t headerEnd = conv.cgiOutput.find("\r\n\r\n");
		if (headerEnd != std::string::npos) //ca veut dire qu'on a trouve les headers !
		{
			std::string headerPart = conv.cgiOutput.substr(0, headerEnd);
			parseCgiHeaders(conv, headerPart);
			sendHeadersToClient(conv);

			std::string bodyPart = conv.cgiOutput.substr(headerEnd + 4);
			if (!bodyPart.empty())
				sendBodyPartToClient(conv, bodyPart, bodyPart.size());

			conv.headersSent = true;
			conv.cgiOutput.clear();
		}
	}
	else
	{
		sendBodyPartToClient(conv, buffer, bytesRead);
		//attention > pas adapte au Multipart data.. la on envoie brut.. donc faut parser
	}
}

void CgiExecutor::executeCgiPost(Conversation& conv)
{
	(void)conv;
}


void CgiExecutor::execute(Conversation& conv)
{
	std::string& method = conv.req.method;

	if (method != "GET" && method != "POST")
		return Executor::sendErrorPage(conv, NOT_IMPLEMENTED);

	if (conv.cgiOut == -1)
	{
		if (method == "GET")
			setupCgiProcessGet(conv);
		// else if (method == "POST")
		// 	setupCgiProcessPost(conv);

		addCgiFdToEpoll(conv.cgiOut, conv);
		return;
	}

	if (method == "GET")
		executeCgiGet(conv);
	else if (method == "POST")
		executeCgiPost(conv);
}




// CAS GET

	//il fait une lecture
	//si il a tout = STATE RESPONSE

	//si il n'a qu'un seul bout:
		// construction ligne HEADER => placee au debut du buffer envoi client
		// envoi au client tout en continuant de lire
		// A la fin met l'etat a IS_SENT (si il a reussi a tout envoyer) ++ CLOSE FDS

// CAS POST :

	// SI NORMAL dans conv.streamState
		//ecrit dans le pipe du CGI
		//lit contenu pipeOutCgi
		//parse
		//envoie ca au responseBuilder --> state = RESPONSE;
		// sortie definitive

	// SI START STREAM

	// construction ligne HEADER => placee au debut du buffer envoi client
	//logique de ecriture / lecture / ecriture client en simultane
	// appele en boucle tant que le processus n'est pas termine (par le Dispatcher tant que l'etat reste a CGI_EXECUTOR)
	// A la fin met l'etat a IS_SENT (si il a reussi a tout envoyer) ++ CLOSE FDS ??


// SAVOIR SI ON PEUT SAVOIR SI LES FDS SONT PRETS ? Le CgiExecutor va etre appele des qu'un fd est pret, mais il faut savoir si les autres sont OK ou pas ?


// LOGIQUE POUR NE PAS APPELER LE MEME CLIENT PLUSIEURS FOIS == vu que l'events manager appelle le Dispatcher ppur chaque client en
// fonctions des fds signales par epoll = il faudrait pas qu'il appelle deux fois le meme client si fdIn et Out sont tous les deux OK + client OK
