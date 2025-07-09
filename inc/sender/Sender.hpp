#ifndef SENDER_HPP
# define SENDER_HPP

# include <unistd.h>
# include <cstring>
# include "webserv_enum.hpp"
# include "Conversation.hpp"

class Sender
{
	private:
		static bool		isAlreadySent(Conversation &conv);
		static ssize_t	trySend(Conversation &conv);
		static void		updateStateAfterSend(Conversation &conv, ssize_t bytes);

	public:
		static void		execute(Conversation& conv);
};

#endif
