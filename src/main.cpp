#include "webserv.hpp"
#include "ConfigParser.hpp"
#include "ConfigStore.hpp"
#include "ServerInitializer.hpp"

int main(int ac, char** av) {

	if (ac != 2) {
		std::cerr << "webserv: error. Only one configuration file is required to launch the server" << std::endl;
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

	ConfigStore configsStore(serversConfigs);
	//ServerInitializer initalizer(configsStore);
}
