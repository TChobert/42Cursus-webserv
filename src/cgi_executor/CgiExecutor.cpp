#include "CgiExecutor.hpp"

CgiExecutor::CgiExecutor(int& epollFd) : _epollFd(epollFd) {}

CgiExecutor::~CgiExecutor(void) {}

void CgiExecutor::execute(Conversation& conv) {

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
}


// SAVOIR SI ON PEUT SAVOIR SI LES FDS SONT PRETS ? Le CgiExecutor va etre appele des qu'un fd est pret, mais il faut savoir si les autres sont OK ou pas ?


// LOGIQUE POUR NE PAS APPELER LE MEME CLIENT PLUSIEURS FOIS == vu que l'events manager appelle le Dispatcher ppur chaque client en 
// fonctions des fds signales par epoll = il faudrait pas qu'il appelle deux fois le meme client si fdIn et Out sont tous les deux OK + client OK
