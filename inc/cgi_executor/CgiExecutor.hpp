#pragma once

#include <cstdlib>
#include <sys/epoll.h>
#include <cstring>
#include <cerrno>
#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "IModule.hpp"

class CgiExecutor : public IModule {

	private:

		int& _epollFd;

		void parseCgiHeaders(Conversation& conv, const std::string& headers);
		void sendHeadersToClient(Conversation& conv);
		void sendBodyPartToClient(Conversation& conv, const std::string& data, size_t len);

	public:

		CgiExecutor(int& epollFd);
		~CgiExecutor(void);
		void execute(Conversation& conv);
		void executeCgiGet(Conversation& conv);
		void executeCgiPost(Conversation& conv);
		void addCgiFdToEpoll(int fd, Conversation& conv);
		void setupCgiProcessGet(Conversation& conv);
		// void setupCgiProcessPost(Conversation& conv);
		char** prepareEnv(Conversation& conv);
};
