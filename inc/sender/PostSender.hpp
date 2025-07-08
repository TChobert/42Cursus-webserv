#ifndef POSTSENDER_HPP
# define POSTSENDER_HPP

# include "Conversation.hpp"

class PostSender
{
	private:
		bool isKeepAlive(const Conversation& conv);
		bool isContinueStatus(const Conversation& conv);

	public:
		static void execute(Conversation& conv);
};

#endif
