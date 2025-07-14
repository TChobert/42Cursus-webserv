#pragma once

#include <sys/epoll.h>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "IModule.hpp"

typedef enum e_interest_mode {

	READ,
	WRITE
};

class Dispatcher {

	private:

	int&		_epollFd;
	IModule*	_reader;
	IModule*	_parser;
	IModule*	_validator;
	IModule*	_executor;
	IModule*	_responseBuilder;
	IModule*	_sender;
	IModule*	_postSender;

	void	setEpollInterest(int clientFd, e_interest_mode mode);
	void	removeClientFromEpoll(Conversation& conv);

	public:

	Dispatcher(int& EpollFd, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender);
	~Dispatcher(void);

	void	dispatch(Conversation& conv);
};
