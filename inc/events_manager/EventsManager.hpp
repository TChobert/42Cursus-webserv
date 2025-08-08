#pragma once

#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdexcept>
#include <map>
#include <set>
#include <ctime>
#include <sys/time.h>

#include "webserv.hpp"
#include "ConfigStore.hpp"
#include "ServerInitializer.hpp"
#include "Dispatcher.hpp"
#include "moduleRegistry.hpp"

#include "webserv_enum.hpp"

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#define MAX_EVENTS 64

class EventsManager {

	private:

	ConfigStore&				_configs;
	ServerInitializer&			_initializer;
	Dispatcher					_dispatcher;
	std::set<int>				_listenSockets;
	std::map<int,Conversation>	_clients;
	std::map<int, Conversation*> _executorFds;
	int&						_epollFd;
	struct epoll_event			_events[MAX_EVENTS];

	IModule* _reader;
	IModule* _parser;
	IModule* _validator;
	IModule* _executor;
	IModule* _responseBuilder;
	IModule* _sender;
	IModule* _postSender;

	void	listenEvents(void);
	void	handleNotifiedEvents(int fdsNumber);
	void	handleNewClient(int fd);
	int		acceptClient(int fd);
	void	setClientConversation(int serverFd, int clientFd);
	void	addClientToInterestList(int clientFd);
	void	setSocketNonBlocking(int fd);
	void	handleClientEvent(int fd);
	void	checkClientsTimeouts(void);
	void	deleteTimeoutsClients(std::vector<Conversation*> timeOutsClients);
	void	closeFinishedClients(void);
	void	deleteClient(Conversation& conv);
	void	deleteAllClients(void);
	void	deleteAllServers(void);
	void	deleteAllNetwork(void);

	public:

	EventsManager(int& epollFd, ConfigStore& configs,
					ServerInitializer& initializer, const moduleRegistry& modules);
	~EventsManager(void);

	void	run(void);
};
