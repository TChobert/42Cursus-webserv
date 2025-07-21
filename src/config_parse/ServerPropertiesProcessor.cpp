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

	int port;
	try {
		port = std::stoi(propertyValue);
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Error: webserv: Invalid PORT property detected in configuration file. Not a integer");
	}
	if (port < 0 || port > 65535 )
		throw InvalidPortPropertyException();
	context->currentConfig.identity.port = static_cast<uint16_t>(port);
}

void ServerPropertiesProcessor::processHostProperty(const std::string& propertyValue, parserContext *context) {

	if (propertyValue.empty()) {
		throw InvalidHostPropertyException();
	}
	context->currentConfig.identity.host = propertyValue;
}

void ServerPropertiesProcessor::processNameProperty(const std::string& propertyValue, parserContext *context) {

	if (propertyValue.empty())
		throw InvalidNamePropertyException();
	size_t i = 0;
	while (i < propertyValue.size()) {
		if (std::isspace(static_cast<unsigned char>(propertyValue[i]))) {
			throw InvalidNamePropertyException();
		}
	}
	context->currentConfig.identity.serverName = propertyValue;
}

void ServerPropertiesProcessor::processRootProperty(const std::string& propertyValue, parserContext *context) {

	if (propertyValue.empty() || propertyValue[0] != '/') {
		throw InvalidServerRootException();
	}
	context->currentConfig.identity.root = propertyValue;
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
	return "Error: webserv: Invalid property detected in configuration file. Out of range (0 - 65535)";
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
