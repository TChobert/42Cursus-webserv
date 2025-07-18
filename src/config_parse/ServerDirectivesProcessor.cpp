#include "ServerDirectivesProcessor.hpp"

ServerDirectivesProcessor::ServerDirectivesProcessor(void) {}

ServerDirectivesProcessor::~ServerDirectivesProcessor(void) {}

ServerDirectivesProcessor::keyType ServerDirectivesProcessor::getKeyType(const std::string& directive) {

	if (directive.compare("port") == 0)
		return (PORT);
	else if (directive.compare("host") == 0)
		return (HOST);
	else if (directive.compare("name") == 0) 
		return (NAME);
	else if (directive.compare("root") == 0)
		return (ROOT);
	else
		return (UNKNOWN);
}

void ServerDirectivesProcessor::processPortDirective(const std::string& directive) {

	(void)directive;
	std::cout << "PORT" << std::endl;
}

void ServerDirectivesProcessor::processHostDirective(const std::string& directive) {

	(void)directive;
	std::cout << "HOST" << std::endl;
}

void ServerDirectivesProcessor::processNameDirective(const std::string& directive) {

	(void)directive;
	std::cout << "NAME" << std::endl;
}

void ServerDirectivesProcessor::processRootDirective(const std::string& directive) {

	(void)directive;
	std::cout << "ROOT" << std::endl;
}

ServerDirectivesProcessor::ProcessPtr ServerDirectivesProcessor::getDirectiveProcess(const std::string& directive) {

	keyType type = getKeyType(directive);

	if (type == UNKNOWN) {
		throw InvalidDirectiveException();
	}
	switch (type) {
		case PORT :
			return &ServerDirectivesProcessor::processPortDirective;
			break ;
		case HOST :
			return &ServerDirectivesProcessor::processHostDirective;
			break ;
		case NAME :
			return &ServerDirectivesProcessor::processNameDirective;
			break ;
		case ROOT :
			return &ServerDirectivesProcessor::processRootDirective;
			break ;
		default:
			throw InvalidDirectiveException();
	}
}
