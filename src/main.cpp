#include "webserv.hpp"
#include "EventsManager.hpp"
#include "IModule.hpp"
#include "ConfigParser.hpp"
#include "ConfigStore.hpp"
#include "ServerInitializer.hpp"
#include "Dispatcher.hpp"
#include "Reader.hpp"

int main(int ac, char** av) {

	if (ac != 2) {
		std::cerr << "webserv: error. Only a configuration file is required to launch the server" << std::endl;
		return (EXIT_FAILURE);
	}

	std::vector<serverConfig> serversConfigs;
	const std::string& configuration = av[1];

	try {
		ConfigFileReader reader;
		ConfigParser configFileParser(configuration, reader);
		serversConfigs = configFileParser.parse();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	int epollFd = epoll_create1(0);
	if (epollFd < 0) {
		std::cerr << "Epoll create failure" << std::endl;
		return (EXIT_FAILURE);
	}
	ConfigStore configsStore(serversConfigs);
	ServerInitializer initalizer(configsStore);

	EventsManager manager()

	return (EXIT_SUCCESS);
}
