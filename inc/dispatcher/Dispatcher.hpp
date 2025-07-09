#pragma once

#include "IModule.hpp"

class Conversation;

class Dispatcher {

	private:

	int&	_epollFd;

	public:

	Dispatcher(const IModule& reader, const IModule& parser, const IModule& validator, const IModule& executor,
		const IModule& responseBuilder, const IModule& sender, const IModule& postSender, const int& EpollFd);
	~Dispatcher(void);

	void	dispatch(const Conversation& context);
};
