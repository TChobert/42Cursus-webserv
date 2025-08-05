#pragma once

#include <iostream>
#include <set>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <sys/epoll.h>
#include <sstream>
#include <fcntl.h>

#include "ConfigStore.hpp"

class ServerInitializer {

	private:

	ConfigStore&	_configs;
	int				_epollFd;

	void			socketInitProcess(int socket, const serverConfig& config);
	void			setSocketImmediatReuse(int socket);
	void			bindSocket(int socket, const serverConfig& config);
	void			setSocketListeningMode(int socket);
	void			setSocketNonBlocking(int socket);
	void			addSocketToEpoll(int socket);

	public:

	ServerInitializer(ConfigStore& configs, int& epollFd);
	~ServerInitializer(void);

	std::set<int>	initServers(void);
};
