#include "ServerDirectivesProcessor.hpp"
#include "ConfigParser.hpp"

ServerDirectivesProcessor::ServerDirectivesProcessor(void) {}

ServerDirectivesProcessor::~ServerDirectivesProcessor(void) {}

ServerDirectivesProcessor::keyType ServerDirectivesProcessor::getKeyType(const std::string& directiveKey) {

	if (directiveKey.compare("port") == 0)
		return (PORT);
	else if (directiveKey.compare("host") == 0)
		return (HOST);
	else if (directiveKey.compare("name") == 0) 
		return (NAME);
	else if (directiveKey.compare("root") == 0)
		return (ROOT);
	else
		return (UNKNOWN);
}

void ServerDirectivesProcessor::processPortDirective(const std::string& directiveValue, parserContext *context) {

	(void)directiveValue;
	(void)context;
	std::cout << "PORT" << std::endl;
}

void ServerDirectivesProcessor::processHostDirective(const std::string& directiveValue, parserContext *context) {

	(void)directiveValue;
	(void)context;
	std::cout << "HOST" << std::endl;
}

void ServerDirectivesProcessor::processNameDirective(const std::string& directiveValue, parserContext *context) {

	(void)directiveValue;
	(void)context;
	std::cout << "NAME" << std::endl;
}

void ServerDirectivesProcessor::processRootDirective(const std::string& directiveValue, parserContext *context) {

	(void)directiveValue;
	(void)context;
	std::cout << "ROOT" << std::endl;
}

ServerDirectivesProcessor::ProcessPtr ServerDirectivesProcessor::getDirectiveProcess(const std::string& directiveKey) {

	keyType type = getKeyType(directiveKey);

	if (type == UNKNOWN) {
		throw InvalidDirectiveException();
	}
	switch (type) {
		case PORT :
			return &ServerDirectivesProcessor::processPortDirective;
		case HOST :
			return &ServerDirectivesProcessor::processHostDirective;
		case NAME :
			return &ServerDirectivesProcessor::processNameDirective;
		case ROOT :
			return &ServerDirectivesProcessor::processRootDirective;
		default:
			throw InvalidDirectiveException();
	}
}

//EXCEPTIONS

const char* ServerDirectivesProcessor::InvalidDirectiveException::what() const throw() {
	return "Invalid directive exception";
}
