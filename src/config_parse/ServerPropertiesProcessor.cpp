#include "ServerPropertiesProcessor.hpp"
#include "ConfigParser.hpp"

ServerPropertiesProcessor::ServerPropertiesProcessor(void) {}

ServerPropertiesProcessor::~ServerPropertiesProcessor(void) {}

ServerPropertiesProcessor::keyType ServerPropertiesProcessor::getKeyType(const std::string& propertyKey) {

	if (propertyKey.compare("port") == 0)
		return (PORT);
	else if (propertyKey.compare("host") == 0)
		return (HOST);
	else if (propertyKey.compare("name") == 0) 
		return (NAME);
	else if (propertyKey.compare("root") == 0)
		return (ROOT);
	else if (propertyKey.compare("error_page") == 0)
		return (ERROR_PAGE);
	else
		return (UNKNOWN);
}

void ServerPropertiesProcessor::processPortProperty(const std::string& propertyValue, parserContext *context) {

	if (context->seenServerProperties.portSeen) {
		throw InvalidPortPropertyException();
	}
	const char *propertyStr = propertyValue.c_str();
	char *endPtr = NULL;

	errno = 0;
	long port = std::strtol(propertyStr, &endPtr, 10);
	if (errno != 0 || endPtr == propertyStr || *endPtr != '\0') {
		throw InvalidPortPropertyException();
	}
	if (port < 0 || port > 65535) {
		throw InvalidPortPropertyException();
	}
	context->seenServerProperties.portSeen = true;
	context->currentConfig.identity.port = static_cast<uint16_t>(port);
}

void ServerPropertiesProcessor::processHostProperty(const std::string& propertyValue, parserContext *context) {

	std::cout << "HOST" << std::endl;
	if (propertyValue.empty()) {
		throw InvalidHostPropertyException();
	}
	context->currentConfig.identity.host = propertyValue;
}

void ServerPropertiesProcessor::processNameProperty(const std::string& propertyValue, parserContext *context) {

	std::cout << "NAME" << std::endl;
	if (propertyValue.empty())
		throw InvalidNamePropertyException();
	size_t i = 0;
	while (i < propertyValue.size()) {
		if (std::isspace(static_cast<unsigned char>(propertyValue[i]))) {
			throw InvalidNamePropertyException();
		}
		++i;
	}
	context->currentConfig.identity.serverName = propertyValue;
}

void ServerPropertiesProcessor::processRootProperty(const std::string& propertyValue, parserContext *context) {

	std::cout << "ROOT" << std::endl;
	if (propertyValue.empty() || propertyValue[0] != '/') {
		throw InvalidServerRootException();
	}
	context->currentConfig.identity.root = propertyValue;
	context->currentConfig.identity.hasRoot = true;
}

ServerPropertiesProcessor::ProcessPtr ServerPropertiesProcessor::getPropertyProcess(const std::string& directiveKey) {

	keyType type = getKeyType(directiveKey);

	if (type == UNKNOWN) {
		throw InvalidPropertyException();
	}
	switch (type) {
		case PORT :
			return &ServerPropertiesProcessor::processPortProperty;
		case HOST :
			return &ServerPropertiesProcessor::processHostProperty;
		case NAME :
			return &ServerPropertiesProcessor::processNameProperty;
		case ROOT :
			return &ServerPropertiesProcessor::processRootProperty;
		default:
			throw InvalidPropertyException();
	}
}

//EXCEPTIONS

const char* ServerPropertiesProcessor::InvalidPropertyException::what() const throw() {
	return "Error: webserv: Invalid property detected in configuration file.";
}

const char *ServerPropertiesProcessor::InvalidPortPropertyException::what() const throw() {
	return "Error: webserv: Invalid port detected in configuration file. Must be an integer in range 0 - 65535";
}

const char *ServerPropertiesProcessor::InvalidHostPropertyException::what() const throw() {
	return "Error: webserv: Invalid host detected in configuration file. Can't be empty";
}

const char *ServerPropertiesProcessor::InvalidNamePropertyException::what() const throw() {
	return "Error: webserv: Invalid server name detected in configuration file. Can't be empty or contains spaces.";
}

const char *ServerPropertiesProcessor::InvalidServerRootException::what() const throw() {
	return "Error: webserv: Invalid server root detected in configuration file. Can't be empty and must be an absolut path.";
}
