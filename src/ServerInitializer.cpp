#include "ServerInitializer.hpp" 

ServerInitializer::ServerInitializer(ConfigStore& configs, int& epollFd) : _configs(configs), _epollFd(epollFd) {}

ServerInitializer::~ServerInitializer(void) {}

void	ServerInitializer::setSocketImmediatReuse(int& socket) {

	int	opt = 1;
	if (setsockopt(socket,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		close (socket);
		throw std::runtime_error("failed to initialized socket immediat reuse. Closing it");
	}
}

void	ServerInitializer::bindSocketToAddress(int& socket, const serverConfig& config ) {

	sockaddr_in	address{};
	//std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(config.identity.port);
	address.sin_addr.s_addr = inet_addr(config.identity.host.c_str());

	if (bind(socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
		close (socket);
		throw std::runtime_error("Failed to bind socket. Closing it");
	}
}

void	ServerInitializer::setSocketListeningMode(int& socket) {

	if (listen(socket, SOMAXCONN) < 0) {
		close(socket);
		throw std::runtime_error("Failed to put socket in listening mode. Closing it");
	}
}

void	ServerInitializer::addSocketToEpoll(int& socket) {

	struct epoll_event	ev;

	ev.events = EPOLLIN;
	ev.data.fd = socket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &ev) < 0) {
		close (socket);
		throw std::runtime_error("Failed to add socket to epoll. Closing it");
	}
}

std::set<int>	ServerInitializer::initServers(void) {

	std::set<int>	serversSockets;
	const std::vector<serverConfig>& configs = _configs.getPreInitConfigs();

	for (std::vector<serverConfig>::const_iterator it = configs.begin(); it != configs.end(); ++it) {

		const serverConfig& config = *it;

		int	sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1) {
			throw std::runtime_error("Failed to initalized socket");
		}
		setSocketImmediatReuse(sock);
		bindSocketToAddress(sock, config);
		setSocketListeningMode(sock);

		bindSocketToAddress(sock, config);
		serversSockets.insert(sock);
	}
	return (serversSockets);
}
