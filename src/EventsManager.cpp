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

EventsManager::~EventsManager(void) {
	//deleteAllNetwork();
}

void	EventsManager::deleteAllNetwork(void) {
	// tout nettoyer (FDS + epoll), puis quitter
}

void EventsManager::handleClientEvent(int fd) {

	auto it = _clients.find(fd);
	if (it != _clients.end()) {
		_dispatcher.dispatch(it->second);
	}
	// secu eventuelle a prevoir si il ne le trouve pas
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
		deleteAllNetwork();
	}
	listenEvents(); // A VOIR SI DANS UN TRY CATCH
}
