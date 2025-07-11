#include "EventsManager.hpp"

EventsManager::EventsManager(ConfigStore& configs,
	ServerInitializer& initializer, Dispatcher& dispatcher) :
	_configs(configs), _initializer(initializer), _dispatcher(dispatcher),
	_listenSockets(), _clients(), _epollFd(-1)
{
	_epollFd = epoll_create1(0);
	if (_epollFd < 0) {
		throw std::runtime_error("Epoll create failure");
	}
}

void	EventsManager::deleteClient(int fd) {
	close(fd);
	_clients.erase(fd);
}

void	EventsManager::deleteAllClients(void) {

	std::map<int, Conversation>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it) {
		deleteClient(it->first);
	}
	_clients.clear();
}

void	EventsManager::deleteServers(void) {

	std::set<int>::iterator it;

	for (it = _listenSockets.begin(); it != _listenSockets.end(); ++it) {
		close(*it);
	}
	_listenSockets.clear();
}

void	EventsManager::deleteAllNetwork(void) {

	deleteAllClients();
	deleteServers();
	close (_epollFd);
}

EventsManager::~EventsManager(void) {
	deleteAllNetwork();
}

void EventsManager::handleClientEvent(int fd) {

	std::map<int, Conversation>::iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		_dispatcher.dispatch(it->second);
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
		if (errno == EAGAIN || errno ==EWOULDBLOCK) {
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
		}
	}
	catch (const std::exception& e) {
		std::cerr << "acceptNewClient error: " << e.what() << std::endl;
	}
}

void	EventsManager::closeFinishedClients(void) {

	std::vector<int>	to_remove;
	std::map<int, Conversation>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); ++it) {
		Conversation&	currentClient = it->second;

		if (currentClient.state == FINISH) {
			to_remove.push_back(it->first);
		}
	}

	for (size_t i = 0; i < to_remove.size(); ++i) {
		deleteClient(to_remove[i]);
	}
}

void	EventsManager::listenEvents(void) {

	while (true) {

		closeFinishedClients();
		int	fdsNumber = epoll_wait(_epollFd, _events, MAX_EVENTS, - 1);
		if (fdsNumber == -1) {
			//gestion d'erreur a definir + SIGNAL HANDLING
		}

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
}

void	EventsManager::run(void) {

	_listenSockets = _initializer.initServers();
	listenEvents();
}
