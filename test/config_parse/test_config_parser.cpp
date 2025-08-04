#include "ConfigParser.hpp"

void printConfig(const serverConfig& config) {

	std::cout << "Has root: " << config.identity.hasRoot << std::endl;
	std::cout << "root: " << config.identity.root << std::endl;
	std::cout << "host: " << config.identity.host << std::endl;
	std::cout << "port: " << config.identity.port << std::endl;
	std::cout << "name: " << config.identity.serverName << std::endl;
}

int main(int ac, char **av) {

	if (ac != 2)
		return (EXIT_FAILURE);

	std::string config = av[1];
	std::vector<serverConfig> configs;

	ConfigFileReader reader;
	HeaderSectionParser headerParser;
	ServerSectionParser serverParser;
	LocationSectionParser locParser;
	ConfigParser parser(config, reader, headerParser, serverParser, locParser);

	parser.parse();
	configs = parser.getConfigs();
	for (std::vector<serverConfig>::const_iterator it = configs.begin(); it != configs.end(); ++it) {
		printConfig(*it);
	}
	return (EXIT_SUCCESS);
}
