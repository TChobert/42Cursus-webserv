#ifndef POSTSENDER_HPP
# define POSTSENDER_HPP

# include "webserv_utils.hpp"
# include "webserv_enum.hpp"
# include "Conversation.hpp"

class Conversation;

class PostSender
{
	private:
		bool isKeepAlive(const Conversation& conv);
		bool isContinueStatus(const Conversation& conv);

	public:
		static void execute(Conversation& conv);
};

#endif
