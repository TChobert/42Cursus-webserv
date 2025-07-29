#include "webserv.hpp"
#include "EventsManager.hpp"
#include "IModule.hpp"
#include "ConfigParser.hpp"
#include "ConfigStore.hpp"
#include "ServerInitializer.hpp"
#include "Dispatcher.hpp"
#include "Reader.hpp"
#include "Parser.hpp"
#include "Validator.hpp"
#include "Executor.hpp"
#include "ResponseBuilder.hpp"
#include "Sender.hpp"
#include "PostSender.hpp"

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
	ConfigStore configStore(serversConfigs);
	ServerInitializer initializer(configStore);
	Reader reader;
	Parser parser;
	Validator validator;
	Executor executor;
	ResponseBuilder responseBuilder;
	Sender sender;
	PostSender postSender;

	EventsManager manager(epollFd, configStore, initializer, &reader, &parser, &validator,
		&executor, &responseBuilder, &sender, &postSender);

	try {
		manager.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
