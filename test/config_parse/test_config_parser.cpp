#include "ConfigParser.hpp"
#include <iostream>

// Helpers
void displayAllowedMethods(const locationConfig& loc) {
	std::cout << "Allowed Methods: ";
	for (size_t i = 0; i < loc.allowedMethods.size(); ++i) {
		std::cout << loc.allowedMethods[i];
		if (i != loc.allowedMethods.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;
}

void displayIndexFiles(const locationConfig& loc) {
	std::cout << "Index files: ";
	for (size_t i = 0; i < loc.indexFiles.size(); ++i) {
		std::cout << loc.indexFiles[i];
		if (i != loc.indexFiles.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;
}

void displayCgiHandlers(const locationConfig& loc) {
	std::cout << "CGI Handlers:" << std::endl;
	for (std::map<cgiExtension, cgiHandler>::const_iterator it = loc.cgiHandlers.begin(); it != loc.cgiHandlers.end(); ++it) {
		std::cout << "  " << it->first << " => " << it->second << std::endl;
	}
}

// Main print function
void printConfig(const serverConfig& config) {
	std::cout << "========== SERVER CONFIG ==========" << std::endl;
	std::cout << "Has root: " << config.identity.hasRoot << std::endl;
	std::cout << "Root: " << config.identity.root << std::endl;
	std::cout << "Host: " << config.identity.host << std::endl;
	std::cout << "Port: " << config.identity.port << std::endl;
	std::cout << "Name: " << config.identity.serverName << std::endl;

	std::cout << "\n---------- LOCATIONS ----------" << std::endl;
	for (std::map<locationName, locationConfig>::const_iterator it = config.locations.begin(); it != config.locations.end(); ++it) {
		const locationConfig& loc = it->second;

		std::cout << "\n[LOCATION: " << loc.name << "]" << std::endl;
		displayAllowedMethods(loc);
		std::cout << "Root: " << loc.root << std::endl;
		std::cout << "Has root: " << loc.hasRoot << std::endl;
		std::cout << "Autoindex: " << (loc.autoIndex ? "on" : "off") << std::endl;
		displayIndexFiles(loc);
		std::cout << "Client max body size: " << loc.clientMaxBodySize << std::endl;

		if (loc.hasRedir) {
			std::cout << "Redirect: " << loc.redirCode << " -> " << loc.redirURL << std::endl;
		} else {
			std::cout << "No redirection" << std::endl;
		}

		std::cout << "Upload enabled: " << (loc.uploadEnabled ? "yes" : "no") << std::endl;
		if (loc.uploadEnabled)
			std::cout << "Upload store: " << loc.uploadStore << std::endl;

		displayCgiHandlers(loc);
	}
	std::cout << "===================================" << std::endl;
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
