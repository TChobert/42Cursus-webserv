#pragma once

#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdexcept>
#include <map>
#include <set>

#include "webserv.hpp"
#include "ConfigStore.hpp"
#include "ServerInitializer.hpp"
#include "Dispatcher.hpp"

#include "webserv_enum.hpp"

#define MAX_EVENTS 64 // arbitraire, a changer ou non

class EventsManager {

	private:

	ConfigStore&				_configs;
	ServerInitializer&			_initializer;
	Dispatcher&					_dispatcher;
	std::set<int>				_listenSockets;
	std::map<int,Conversation>	_clients;
	int							_epollFd;
	struct epoll_event			_events[MAX_EVENTS];

	void	listenEvents(void);
	void	handleNotifiedEvents(int fdsNumber);
	void	handleNewClient(int fd);
	int		acceptClient(int fd);
	void	setClientConversation(int serverFd, int clientFd);
	void	addClientToInterestList(int clientFd);
	void	setSocketNonBlocking(int fd);
	void	handleClientEvent(int fd);
	void	closeFinishedClients(void);
	void	deleteClient(int fd);
	void	deleteAllClients(void);
	void	deleteAllServers(void);
	void	deleteAllNetwork(void);

	public:

	EventsManager(ConfigStore& configs, ServerInitializer& initializer, Dispatcher& dispatcher);
	~EventsManager(void);

	void	run(void);
};
