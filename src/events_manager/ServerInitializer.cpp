#include "ServerInitializer.hpp"

ServerInitializer::ServerInitializer(ConfigStore& configs, int& epollFd) : _configs(configs), _epollFd(epollFd) {}

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

	const std::string& ip = config.identity.host;
	const int port = config.identity.port;

	struct addrinfo hints;
	struct addrinfo *res;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::ostringstream portStr;
	portStr << port;

	int ret = getaddrinfo((ip == "*" ? NULL : ip.c_str()), portStr.str().c_str(), &hints, &res);
	if (ret != 0 || !res) {
		throw std::runtime_error("getaddrinfo() failed for " + ip + ":" + portStr.str() + ": " + gai_strerror(ret));
	}
	if (bind(socket, res->ai_addr, res->ai_addrlen) < 0) {
		freeaddrinfo(res);
		throw std::runtime_error("bind() failed on " + ip + ":" + portStr.str() + ": " + strerror(errno));
	}
	freeaddrinfo(res);
}

void	ServerInitializer::setSocketListeningMode(int socket) {

	if (listen(socket, SOMAXCONN) < 0) {
		std::ostringstream	oss;
		oss << "Failed to put socket: " << socket << " on listening mode. Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::addSocketToEpoll(int socket) {

	struct epoll_event	ev;

	ev.events = EPOLLIN;
	ev.data.fd = socket;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, socket, &ev) < 0) {
		std::ostringstream	oss;
		oss << "Failed to add socket: " << socket << ". Closing it.";
		throw std::runtime_error(oss.str());
	}
}

void	ServerInitializer::socketInitProcess(int socket, const serverConfig& config) {

	setSocketImmediatReuse(socket);
	setSocketNonBlocking(socket);
	bindSocket(socket, config);
	setSocketListeningMode(socket);
	addSocketToEpoll(socket);

	_configs.bindSocketToConfig(socket, config);
}

std::set<int>	ServerInitializer::initServers(void) {

	std::set<int>	serversSockets;
	const std::vector<serverConfig>& configs = _configs.getPreInitConfigs();

	for (std::vector<serverConfig>::const_iterator it = configs.begin(); it != configs.end(); ++it) {

		const serverConfig& config = *it;

		int	sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock >= 0) {
			try {
				socketInitProcess(sock, config);
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
