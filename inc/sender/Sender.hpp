#ifndef SENDER_HPP
# define SENDER_HPP

# include <unistd.h>
# include <cstring>
#include "webserv.hpp"
# include "webserv_enum.hpp"
# include "IModule.hpp"

class Sender : public IModule
{
	private:
		static ssize_t	trySend(Conversation &conv);
		static void		updateStateAfterSend(Conversation &conv, ssize_t bytes);

	public:
		void			execute(Conversation& conv);
};

#endif
