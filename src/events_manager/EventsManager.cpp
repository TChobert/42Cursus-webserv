#include "EventsManager.hpp"
extern volatile sig_atomic_t gSignalStatus;

EventsManager::EventsManager(int& epollFd, ConfigStore& configs,
								ServerInitializer& initializer,
								const moduleRegistry& modules)
	: _epollFd(epollFd),
	_configs(configs),
	_initializer(initializer),
	_listenSockets(),
	_clients(),
	_executorFds(),
	_dispatcher(epollFd, _clients, _executorFds,
		modules.reader, modules.parser, modules.validator,
		modules.executor, modules.responseBuilder,
		modules.sender, modules.postSender),
	_reader(modules.reader),
	_parser(modules.parser),
	_validator(modules.validator),
	_executor(modules.executor),
	_responseBuilder(modules.responseBuilder),
	_sender(modules.sender),
	_postSender(modules.postSender) {}

EventsManager::~EventsManager(void) {
	deleteAllNetwork();
}

void	EventsManager::deleteClient(Conversation& client) {

	if (client.tempFd != -1) {
		close(client.tempFd);
		_executorFds.erase(client.tempFd);
		client.tempFd = -1;
	}
	if (client.fdToClose != -1) {
		close(client.fdToClose);
		_executorFds.erase(client.fdToClose);
		client.fdToClose = -1;
	}
	std::cout << RED << "[client] =" << "= " << client.fd << "==> is now removed from network." << RESET << std::endl;
	close(client.fd);
	_clients.erase(client.fd);
}

void EventsManager::deleteAllClients(void) {

	std::map<int, Conversation> toDelete;
	toDelete.swap(_clients);
	for (std::map<int, Conversation>::iterator it = toDelete.begin(); it != toDelete.end(); ++it) {
		deleteClient(it->second);
	}
}

void	EventsManager::deleteAllServers(void) {

	std::set<int>::iterator it;

	for (it = _listenSockets.begin(); it != _listenSockets.end(); ++it) {
		close(*it);
	}
	_listenSockets.clear();
}

void	EventsManager::deleteAllNetwork(void) {

	std::cout << RED << "[ !! DELETING ALL NETWORK !! ]" << RESET << std::endl;
	deleteAllClients();
	deleteAllServers();
	if (_epollFd >= 0)
		close (_epollFd);
}

void EventsManager::handleClientEvent(int fd) {

	std::map<int, Conversation*>::iterator execIt = _executorFds.find(fd);
	if (execIt != _executorFds.end()) {
		updateClientLastActivity(*execIt->second, REGULAR);
		_dispatcher.dispatch(*execIt->second);
	} else {
		std::map<int, Conversation>::iterator clientIt = _clients.find(fd);
		if (clientIt != _clients.end()) {
			std::cout << "Client found !!" << std::endl;
			updateClientLastActivity(clientIt->second, REGULAR);
			_dispatcher.dispatch(clientIt->second);
		}
	}
}

void	EventsManager::setSocketNonBlocking(int fd) {

	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		close(fd);
		std::ostringstream	oss;
		oss << "fcntl failed on client fd: " << fd << " while setting it nonblocking, closing it.";
		throw std::runtime_error(oss.str());
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		close(fd);
		std::ostringstream	oss;
		oss << "fcntl failed on client: " << fd << " while setting it nonblocking, closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	EventsManager::setClientConversation(int serverFd, int clientFd) {

	Conversation	clientConversation;
	serverConfig	config = _configs.getConfig(serverFd);

	clientConversation.config = config;
	clientConversation.fd = clientFd;
	updateClientLastActivity(clientConversation, REGULAR);

	_clients[clientFd] = clientConversation;
}

void	EventsManager::addClientToInterestList(int clientFd) {

	struct	epoll_event	ev;
	ev.data.fd = clientFd;
	ev.events = EPOLLIN;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ev) < 0) {
		close(clientFd);
		std::ostringstream	oss;
		oss << "Failed to add client socket " << clientFd << " to epoll; closing it.";
		throw std::runtime_error(oss.str());
	}
}

int	EventsManager::acceptClient(int serverFd) {

	sockaddr_in	clientAddress;
	std::memset(&clientAddress, 0, sizeof(clientAddress));
	socklen_t	clientLen = sizeof(clientAddress);

	int	clientFd = accept (serverFd, (sockaddr *)&clientAddress, &clientLen);
	if (clientFd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return (-1);
		}
		std::ostringstream	oss;
		oss << "Failed to add client to socket: " << serverFd;
		throw std::runtime_error(oss.str());
	}
	setSocketNonBlocking(clientFd);
	return (clientFd);
}

void	EventsManager::handleNewClient(int serverFd) {

	try {
		int	clientFd = acceptClient(serverFd);
		if (clientFd != -1) {
			setClientConversation(serverFd, clientFd);
			addClientToInterestList(clientFd);
			std::cout << GREEN << "[client] =" << "= " << clientFd << " ==> is now connected with [SERVER] ==> " << serverFd << RESET << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "acceptNewClient error: " << e.what() << std::endl;
	}
}

void EventsManager::deleteTimeoutsClients(std::vector<int> timeOutsClients) {
	for (std::vector<int>::iterator it = timeOutsClients.begin(); it != timeOutsClients.end(); ++it) {
		std::cout << RED << "TIMEOUT" << RESET;
		deleteClient(_clients[*it]);
	}
}

void EventsManager::checkClientsTimeouts(void) {
	std::cout << RED << "CHECKING FOR TIMEOUTS" << RESET << std::endl;

	std::vector<int> timeOutsClients;

	for (std::map<int, Conversation>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (isClientTimeOut(it->second)) {
			timeOutsClients.push_back(it->first);
		}
	}

	deleteTimeoutsClients(timeOutsClients);
}

void EventsManager::closeFinishedClients(void) {

	std::vector<int> toRemove;

	for (std::map<int, Conversation>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.state == FINISH) {
			toRemove.push_back(it->first);
		}
	}
	for (size_t i = 0; i < toRemove.size(); ++i) {
		deleteClient(_clients[toRemove[i]]);
	}
}

void	EventsManager::handleNotifiedEvents(int fdsNumber) {

	for (int i = 0; i < fdsNumber; ++i) {

		int currentFd = _events[i].data.fd;

		if (_listenSockets.count(currentFd) > 0) {
			handleNewClient(currentFd);
		}
		else {
			handleClientEvent(currentFd);
		}
	}
}

void	EventsManager::listenEvents(void) {

	while (gSignalStatus == 0) {

		closeFinishedClients();
		checkClientsTimeouts();
		int	fdsNumber = epoll_wait(_epollFd, _events, MAX_EVENTS, 1000);
		if (fdsNumber == -1) {
			if (errno == EINTR) {
				continue ;
			}
			else {
				deleteAllNetwork();
				throw std::runtime_error("Critical epoll_wait error. Server interruption.");
			}
		}
		handleNotifiedEvents(fdsNumber);
	}
}

void	EventsManager::run(void) {

	_listenSockets = _initializer.initServers(_epollFd);
	listenEvents();
}
