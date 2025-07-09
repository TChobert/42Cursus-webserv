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

	std::map<int, Conversation>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		_dispatcher.dispatch(it->second);
	}
	else {
		std::cerr << fd << ": unknown client" << std::endl;
	}
}

void	EventsManager::acceptNewClient(int serverFd) {

	try {
		Conversation	clientConversation;
		serverConfig	config = _configs.getConfig(serverFd);

		clientConversation._config = config;

		sockaddr_in	clientAddress;
		std::memset(&clientAddress, 0, sizeof(clientAddress));
		socklen_t	clientLen = sizeof(clientAddress);

		int	clientFd = accept (serverFd, (sockaddr *)&clientAddress, &clientLen);
		if (clientFd < 0) {
			std::ostringstream	oss;
			oss << "Failed to add client to socket: " << serverFd;
			throw std::runtime_error(oss.str());
		}
		clientConversation.fd = clientFd;
		_clients[clientFd] = clientConversation;
	}
	catch (const std::exception& e) {
		std::cerr << "acceptNewClient error: " << e.what() << std::endl;
	}
}

void	EventsManager::listenEvents(void) {

	while (true) {

		int	fdsNumber = epoll_wait(_epollFd, _events, MAX_EVENTS, - 1);
		if (fdsNumber == -1) {
			//gestion d'erreur a definir + SIGNAL HANDLING
		}

		for (int i = 0; i < fdsNumber; ++i) {

			int currentFd = _events[i].data.fd;

			if (_listenSockets.count(currentFd) > 0) {
				acceptNewClient(currentFd);
			}
			else {
				handleClientEvent(currentFd);
			}
		}
	}
}

void	EventsManager::run(void) {

	try {
		_listenSockets = _initializer.initServers();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		//deleteAllNetwork();
		// EXIT ??
	}
	listenEvents(); // A VOIR SI DANS UN TRY CATCH
}
