#include "Sender.hpp"
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <cstring>

/* ---------------- PRIVATE METHODS ------------------ */

bool	Sender::isAlreadySent(Conversation &conv)
{
	return conv.bytesSent >= conv.final_response.size();
}

ssize_t	Sender::trySend(Conversation &conv)
{
	const std::string& responseData = conv.final_response;
	size_t alreadySent = conv.bytesSent;
	size_t bytesRemaining = responseData.size() - alreadySent;

	//send() retourne le nb d'octets reellement envoyes (peut-etre moins !!)
	//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	//-1 en cas d'erreur > faut check errno
	//std::string::data() donne un const char * vers le debut du buffer de la string
	ssize_t bytesSentNow = ::send(conv.client_fd,
									responseData.data() + alreadySent, // pointeur vers la partie non envoyee.
									bytesRemaining, //longueur de ce qu'il reste a envoyer
									0); //flag a 0 = comportement normal

	 if (bytesSentNow < 0) // cas ou send a echoue..
	 {
		if (errno == EAGAIN || errno == EWOULDBLOCK) //je doit attendre EPOLLOUT ("socket n'est pas pret a ecrire maintenant")
			return -1; //a voir si je dois rendre autre chose ou pas ? (on doit pouvoir dire: "pas envoye maintenant, mais pas grave")

		//ATTENTION: message de debug, a enlever a la fin normalement..
		std::cerr << "[Sender] Erreur send() sur fd " << conv.client_fd
				<< " : " << strerror(errno) << std::endl;

		conv.state = FINISH;
		return -1;
	}

	return bytesSentNow;
}

void	Sender::updateState(Conversation &conv, ssize_t bytesSentNow)
{
	 if (bytesSentNow <= 0)
		return ; //gestion d'erreur dans trySend...

	conv.bytesSent += static_cast<size_t>(bytesSentNow);

	 //protection supplementaire si c'est accidentellement plus eleve...
	if (conv.bytesSent > conv.final_response.size())
		conv.bytesSent = conv.final_response.size();

	if (conv.bytesSent == conv.final_response.size())
		conv.state = IS_SENT;
	else
		conv.state = WRITE_CLIENT; // reste des donnees a envoyer
}


/* ---------------- PUBLIC METHODS ------------------ */

void Sender::sendResponse(Conversation& conv)
{
	if (conv.final_response.empty() || isAlreadySent(conv))
	{
		conv.state = IS_SENT;
		return;
	}

	ssize_t bytesSentNow = trySend(conv);

	if (bytesSentNow == -1)
		return;

	updateState(conv, bytesSentNow);
}
