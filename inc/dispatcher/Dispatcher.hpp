#pragma once

#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "IModule.hpp"

class Conversation;

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

	void	setInterest(int mode);

	public:

	Dispatcher(int& EpollFd, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender);
	~Dispatcher(void);

	void	dispatch(Conversation& conv);
};
