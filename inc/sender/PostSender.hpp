#ifndef POSTSENDER_HPP
# define POSTSENDER_HPP

# include "webserv.hpp"
# include "webserv_utils.hpp"
# include "webserv_enum.hpp"
# include "webserv.hpp"
# include "IModule.hpp"

class Conversation;

class PostSender : public IModule
{
	private:
		bool isKeepAlive(const Conversation& conv);
		bool isContinueStatus(const Conversation& conv);

	public:
		void execute(Conversation& conv);
};

#endif
