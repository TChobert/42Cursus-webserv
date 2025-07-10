#include "Sender.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

ssize_t	Sender::trySend(Conversation &conv)
{
	const std::string& responseData = conv.finalResponse;
	size_t alreadySent = conv.bytesSent;
	size_t bytesRemaining = responseData.size() - alreadySent;

	ssize_t bytesSentNow = ::send(conv.client_fd,
									responseData.data() + alreadySent,
									bytesRemaining,
									0);

	 if (bytesSentNow <= 0)
		return -1;

	return bytesSentNow;
}

void	Sender::updateStateAfterSend(Conversation &conv, ssize_t bytesSentNow)
{
	conv.bytesSent += static_cast<size_t>(bytesSentNow);

	if (conv.bytesSent == conv.finalResponse.size())
		conv.state = IS_SENT;
	else
		conv.state = WRITE_CLIENT;
}


/* ---------------- PUBLIC METHODS ------------------ */

void Sender::execute(Conversation& conv)
{
	if (conv.finalResponse.empty())
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
