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

/* ---------------- PRIVATE METHODS ------------------ */

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
	envStrings.push_back("SERVER_NAME=" + conv.config.identity.server_name);
	envStrings.push_back("SERVER_PORT=" + std::to_string(conv.config.identity.port));

	char** envp = (char**)malloc(sizeof(char*) * (envStrings.size() + 1));
	if (!envp)
		return NULL;
	for (size_t i = 0; i < envStrings.size(); ++i)
	{
		envp[i] = strdup(envStrings[i].c_str());
		if (!envp[i])
		{
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

	//recuperer la bonne extension du script a faire (.py, .php, etc)
	std::string extension = getFileExtension(scriptPath);

	//recuperer le bon interpreteur par rapport a extension, exemple, .py >> python3
	std::map<cgiExtension, cgiHandler>::const_iterator it = conv.location->cgiHandlers.find(extension);
	if (it == conv.location->cgiHandlers.end())
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	const std::string& interpreter = it->second;

	//creation pipe pour que Webserv puisse lire la sortie du script (stockee dans le pipe)
	int pipe_out[2];
	if (pipe(pipe_out) == -1)
		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);

	pid_t pid = fork();
	if (pid < 0)
	{
		close(pipe_out[0]);
		close(pipe_out[1]);
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	//but process enfant: rediriger sortie stdout vers pipe
	if (pid == 0)
	{
		close(pipe_out[0]);
		if (dup2(pipe_out[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE); //kill processus enfant > waitpid parent avec erreur 500
		close(pipe_out[1]);

		char** envp = prepareEnv(conv);
		if (!envp)
			exit(EXIT_FAILURE);

		//{interpreter, scriptPath, NULL}
		char* argv[] = {const_cast<char*>(interpreter.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
		execve(interpreter.c_str(), argv, envp);
		freeEnv(envp);
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		close(pipe_out[1]);
		conv.tempFd = pipe_out[0];
		conv.eState = READ_EXEC_GET_CGI;
		conv.state = READ_EXEC;
	}
}

void	CGIHandler::handlePostCGI(Conversation& conv)
{

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
