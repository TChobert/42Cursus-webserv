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
	//std::string::data() donne un const char * vers le debut du buffer de la string
	ssize_t bytesSentNow = ::send(conv.client_fd,
									responseData.data() + alreadySent, // pointeur vers la partie non envoyee.
									bytesRemaining, //longueur de ce qu'il reste a envoyer
									0); //flag a 0 = comportement normal

	 if (bytesSentNow <= 0) // cas ou send a echoue..
		return -1;
	//send == 0, c'est possible : rien n'a ete envoye + aucune erreur bloquante levee
	//arrive si socket est connecte, mais ferme de l'autre cote ou shutdown partiel SOCK_STREAM

	return bytesSentNow;
}

void	Sender::updateStateAfterSend(Conversation &conv, ssize_t bytesSentNow)
{
	conv.bytesSent += static_cast<size_t>(bytesSentNow);

	 //protection supplementaire si c'est accidentellement plus eleve...
	if (conv.bytesSent > conv.final_response.size())
		conv.bytesSent = conv.final_response.size();
	if (conv.bytesSent == conv.final_response.size())
		conv.state = IS_SENT;
	else
		conv.state = WRITE_CLIENT; // reste des donnees a envoyer >> attention dispatcher
}


/* ---------------- PUBLIC METHODS ------------------ */

void Sender::execute(Conversation& conv)
{
	if (conv.final_response.empty() || isAlreadySent(conv))
	{
		conv.state = IS_SENT;
		return;
	}

	ssize_t bytesSentNow = trySend(conv);

	if (bytesSentNow <= 0)
	{
		conv.state = FINISH;
		return;
	}

	updateStateAfterSend(conv, bytesSentNow);
}
