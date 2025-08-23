#pragma once

#include <cstdlib>
#include <sys/epoll.h>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cerrno>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "IModule.hpp"

enum e_interest_mode {

	READ,
	WRITE,
	READ_EXEC_FD,
	WRITE_EXEC_FD
};

class Dispatcher {

	private:

	int&		_epollFd;
	std::map<int, Conversation>& _clientsFds;
	std::map<int, Conversation*>& _executorFds;
	IModule*	_reader;
	IModule*	_parser;
	IModule*	_validator;
	IModule*	_executor;
	IModule*	_responseBuilder;
	IModule*	_sender;
	IModule*	_postSender;
	IModule*	_cgiExecutor;

	bool	isInExecutorFdsMap(const int& fd) const;
	void	setExecutorInterest(Conversation& conv, e_interest_mode mode);
	void	setClientInterest(Conversation& conv, e_interest_mode mode);
	void	setEpollInterest(Conversation& conv, e_interest_mode mode);
	void	removeClientFromEpoll(Conversation& conv);
	void	removeExecutorFdFromEpoll(Conversation& conv, int executorFd);
	void	setTimeoutResponse(Conversation& conv);

	public:

	Dispatcher(int& EpollFd, std::map<int, Conversation>& clientsFds, std::map<int, Conversation*>& executorFds, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender, IModule* cgiExecutor);
	~Dispatcher(void);

	void	dispatch(Conversation& conv);
};
