#include "webserv.hpp"
#include "EventsManager.hpp"
#include "IModule.hpp"
#include "ConfigParser.hpp"
#include "HeaderSectionParser.hpp"
#include "ServerSectionParser.hpp"
#include "LocationSectionParser.hpp"
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
#include "moduleRegistry.hpp"

volatile sig_atomic_t gSignalStatus = 0;

static void buildModuleRegistery(moduleRegistry& registry,
							IModule *reader, IModule *parser, IModule *validator,
							IModule *executor, IModule *responseBuilder,
							IModule *sender, IModule *postSender) {
	registry.reader = reader;
	registry.parser = parser;
	registry.validator = validator;
	registry.executor = executor;
	registry.responseBuilder = responseBuilder;
	registry.sender = sender;
	registry.postSender = postSender;
}

int main(int ac, char** av) {

	if (ac != 2) {
		std::cerr << "webserv: error. Only a configuration file is required to launch the server" << std::endl;
		return (EXIT_FAILURE);
	}

	std::signal(SIGINT, signalHandler);
	std::signal(SIGTERM, signalHandler);

	std::vector<serverConfig> serversConfigs;
	const std::string configuration = av[1];
	ConfigFileReader configReader;
	HeaderSectionParser hSectionParser;
	ServerSectionParser sSectionParser;
	LocationSectionParser lSectionParser;
	ConfigParser configFileParser(configuration, configReader, hSectionParser, sSectionParser, lSectionParser);

	try {
		configFileParser.parse();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	serversConfigs = configFileParser.getConfigs();

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
	moduleRegistry modules;

	buildModuleRegistery(modules, &reader, &parser, &validator, &executor, &responseBuilder, &sender, &postSender);

	try {
		EventsManager manager(epollFd, configStore, initializer, modules);
		manager.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
