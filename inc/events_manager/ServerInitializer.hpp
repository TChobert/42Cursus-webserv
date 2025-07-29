#pragma once

#include <iostream>
#include <set>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sstream>
#include <fcntl.h>

#include "ConfigStore.hpp"

class ServerInitializer {

	private:

	ConfigStore&	_configs;

	void			socketInitProcess(int epollFd, int socket, const serverConfig& config);
	void			setSocketImmediatReuse(int socket);
	void			bindSocket(int socket, const serverConfig& config);
	void			setSocketListeningMode(int socket);
	void			setSocketNonBlocking(int socket);
	void			addSocketToEpoll(int epollFd, int socket);

	public:

	ServerInitializer(ConfigStore& configs);
	~ServerInitializer(void);

	std::set<int>	initServers(int epollFd);
};
