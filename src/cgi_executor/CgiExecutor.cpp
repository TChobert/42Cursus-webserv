#include "CgiExecutor.hpp"
#include "Executor.hpp"
#include "webserv.hpp"
#include "webserv_utils.hpp"
#include <string>

CgiExecutor::CgiExecutor(int& epollFd) : _epollFd(epollFd) {}

CgiExecutor::~CgiExecutor(void) {}

void CgiExecutor::executeCgiGet(Conversation& conv)
{
	char buffer[1024];
	ssize_t bytesRead = read(conv.cgiOut, buffer, sizeof(buffer));

	if (!conv.headersSent)
	{
		conv.cgiOutput.append(buffer, bytesRead);

		size_t headerEnd = conv.cgiOutput.find("\r\n\r\n");
		if (headerEnd != std::string::npos) //ca veut dire qu'on a trouve les headers !
		{
			std::string headerPart = conv.cgiOutput.substr(0, headerEnd);
			parseCgiHeaders(conv, headerPart);
			sendHeadersToClient(conv);
		}

		std::string bodyPart = conv.cgiOutput.substr(headerEnd + 4);
		if (!bodyPart.empty())
			sendBodyPartToClient(conv, bodyPart, bodyPart.size());
		//attention > pas adapte au Multipart data.. la on envoie brut.. donc faut parser

		conv.headersSent = true;
		conv.cgiOut.clear();
	}
	else
	{
		sendBodyPartToClient(conv, buffer, bytesRead);
		//attention > pas adapte au Multipart data.. la on envoie brut.. donc faut parser
	}
}

// void CgiExecutor::executeCgiPost(Conversation& conv)
// {

// }


void CgiExecutor::execute(Conversation& conv)
{
	std::string& method = conv.req.method;

	if (method == "GET")
		executeCgiGet(conv);
	else if (method == "POST")
		executeCgiPost(conv);
	else
		Executor::setResponse(conv, NOT_IMPLEMENTED);
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
