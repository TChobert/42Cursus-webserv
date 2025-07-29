#include "ServerInitializer.hpp"

ServerInitializer::ServerInitializer(ConfigStore& configs) : _configs(configs) {}

ServerInitializer::~ServerInitializer(void) {}

void	ServerInitializer::setSocketImmediatReuse(int socket) {

	int	opt = 1;
	if (setsockopt(socket,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::ostringstream	oss;
		oss << "Failed to initialize socket: " << socket << " in immediate reuse mode. Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::setSocketNonBlocking(int socket) {

	int	flags = fcntl(socket, F_GETFL, 0);
	if (flags < 0) {
		std::ostringstream	oss;
		oss << "fcntl failed on server socket: " << socket << " while setting it nonblocking";
		throw std::runtime_error(oss.str());
	}
	if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::ostringstream	oss;
		oss << "fcntl failed on client: " << socket << " while setting it nonblocking";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::bindSocket(int socket, const serverConfig& config ) {

	sockaddr_in	address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(config.identity.port);
	address.sin_addr.s_addr = inet_addr(config.identity.host.c_str());

	if (bind(socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
		std::ostringstream oss;
		oss << "Failed to bind socket on " << config.identity.host << ":" << config.identity.port << ". Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::setSocketListeningMode(int socket) {

	if (listen(socket, SOMAXCONN) < 0) {
		std::ostringstream	oss;
		oss << "Failed to put socket: " << socket << " on listening mode. Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::addSocketToEpoll(int epollFd, int socket) {

	struct epoll_event	ev;

	ev.events = EPOLLIN;
	ev.data.fd = socket;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socket, &ev) < 0) {
		std::ostringstream	oss;
		oss << "Failed to add socket: " << socket << ". Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::socketInitProcess(int epollFd, int socket, const serverConfig& config) {

	setSocketImmediatReuse(socket);
	setSocketNonBlocking(socket);
	bindSocket(socket, config);
	setSocketListeningMode(socket);
	addSocketToEpoll(epollFd, socket);

	_configs.bindSocketToConfig(socket, config);
}

std::set<int>	ServerInitializer::initServers(int epollFd) {

	std::set<int>	serversSockets;
	const std::vector<serverConfig>& configs = _configs.getPreInitConfigs();

	for (std::vector<serverConfig>::const_iterator it = configs.begin(); it != configs.end(); ++it) {

		const serverConfig& config = *it;

		int	sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock >= 0) {
			try {
				socketInitProcess(epollFd, sock, config);
				serversSockets.insert(sock);
			}
			catch (const std::exception& e) {
				close(sock);
				std::cerr << "Error while initializing server " << config.identity.host << ":"
				<< config.identity.port << " → " << e.what() << std::endl;
			}
		}
		else {
			std::cerr << "Failed to initialize server known as: " << config.identity.host << std::endl;
		}
	}
	return (serversSockets);
}
