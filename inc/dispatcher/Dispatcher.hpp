#pragma once

#include <cstdlib>
#include <sys/epoll.h>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <string.h>

#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "IModule.hpp"

typedef enum e_interest_mode {

	READ,
	WRITE,
	READ_EXEC_FD,
	WRITE_EXEC_FD
};

class Dispatcher {

	private:

	int&		_epollFd;
	std::map<int, Conversation>& _executorFds;
	IModule*	_reader;
	IModule*	_parser;
	IModule*	_validator;
	IModule*	_executor;
	IModule*	_responseBuilder;
	IModule*	_sender;
	IModule*	_postSender;

	bool	isInExecutorFdsMap(const int& fd) const;
	void	setExecutorInterest(Conversation& conv, e_interest_mode mode);
	void	setClientInterest(Conversation& conv, e_interest_mode mode);
	void	setEpollInterest(Conversation& conv, e_interest_mode mode);
	void	removeClientFromEpoll(Conversation& conv);

	public:

	Dispatcher(int& EpollFd, std::map<int, Conversation>& executorFds, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender);
	~Dispatcher(void);

	void	dispatch(Conversation& conv);
};
