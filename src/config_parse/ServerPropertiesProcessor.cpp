#include "ServerPropertiesProcessor.hpp"
#include "ConfigParser.hpp"

ServerPropertiesProcessor::ServerPropertiesProcessor(void) {}

ServerPropertiesProcessor::~ServerPropertiesProcessor(void) {}

const int ServerPropertiesProcessor::validErrorCodes[] = {401, 404, 500};

ServerPropertiesProcessor::keyType ServerPropertiesProcessor::getKeyType(const std::string& propertyKey) {

	if (propertyKey.compare("port") == 0)
		return (PORT);
	else if (propertyKey.compare("host") == 0)
		return (HOST);
	else if (propertyKey.compare("name") == 0) 
		return (NAME);
	else if (propertyKey.compare("root") == 0)
		return (ROOT);
	else if (propertyKey.compare("error_page") == 0) {
		return (ERROR_PAGE);
	}
	else
		return (UNKNOWN);
}

int ServerPropertiesProcessor::getErrorCodeValue(const std::string& errorCode) {

	const char *codeStr = errorCode.c_str();
	char *endPtr = NULL;

	errno = 0;
	long code = std::strtol(codeStr, &endPtr, 10);
	if (errno != 0 || endPtr == codeStr || *endPtr != '\0') {
		throw InvalidErrorPageException();
	}
	for (size_t i = 0; i < sizeof(validErrorCodes); ++ i) {
		if (code == validErrorCodes[i]) {
			return (static_cast<int>(code));
		}
	}
	throw InvalidErrorPageException();
	return (0);
}

bool ServerPropertiesProcessor::isErrorPathValid(const std::string& pagePath) {

	if (pagePath.find("..") != std::string::npos || pagePath.empty()) {
		return false;
	}
	if (pagePath[0] == '/') {
		return (false);
	}
	std::ifstream pageStream(pagePath.c_str());
	if (!pageStream.good()) {
	}
	return (pageStream.good());
}

bool ServerPropertiesProcessor::isValueInMap(const std::map<int, std::string>& map, const std::string& value) {
	for (std::map<int, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
		if (it->second == value)
			return true;
	}
	return false;
}

void ServerPropertiesProcessor::addErrorPageToErrorMap(parserContext *context, int codeValue, std::string pagePath) {

	if (context->currentConfig.errorPagesCodes.find(codeValue) != context->currentConfig.errorPagesCodes.end()) {
		throw DoublePropertyException();
	}
	if (isValueInMap(context->currentConfig.errorPagesCodes, pagePath)) {
		throw DoublePropertyException();
	}
	context->currentConfig.errorPagesCodes[codeValue] = pagePath;
}

void ServerPropertiesProcessor::processPortProperty(const std::string& propertyValue, parserContext *context) {

	if (context->seenServerProperties.portSeen) {
		throw DoublePropertyException();
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

	if (context->seenServerProperties.hostSeen) {
		throw DoublePropertyException();
	}
	if (propertyValue.empty()) {
		throw InvalidHostPropertyException();
	}
	context->currentConfig.identity.host = propertyValue;
	context->seenServerProperties.hostSeen = true;
}

void ServerPropertiesProcessor::processNameProperty(const std::string& propertyValue, parserContext *context) {

	if (context->seenServerProperties.nameSeen) {
		throw DoublePropertyException();
	}
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
	context->seenServerProperties.nameSeen =true;
}

void ServerPropertiesProcessor::processRootProperty(const std::string& propertyValue, parserContext *context) {

	if (context->seenServerProperties.rootSeen) {
		throw DoublePropertyException();
	}
	if (propertyValue.empty() || propertyValue[0] != '/') {
		throw InvalidServerRootException();
	}
	context->currentConfig.identity.root = propertyValue;
	context->currentConfig.identity.hasRoot = true;
	context->seenServerProperties.rootSeen = true;
}

void ServerPropertiesProcessor::processErrorPageProperty(const std::string& propertyValue, parserContext *context) {

	std::istringstream iss(propertyValue);

	std::string code, pagePath;
	iss >> code >> pagePath;

	if (code.empty() || pagePath.empty() || code.size() > 3) {
		throw InvalidErrorPageException();
	}
	int codeValue = getErrorCodeValue(code);
	if (isErrorPathValid(pagePath)) {
		addErrorPageToErrorMap(context, codeValue, pagePath);
	}
	else {
		throw InvalidErrorPageException();
	}
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
		case ERROR_PAGE :
		{
			return &ServerPropertiesProcessor::processErrorPageProperty;
		}
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

const char *ServerPropertiesProcessor::InvalidErrorPageException::what() const throw() {
	return "Error: webserv: Invalid error page path property detected. Must be at format: error_page=404 error/404.html. Absolute paths are not tolerated";
}

const char *ServerPropertiesProcessor::DoublePropertyException::what() const throw() {
	return "Error: webserv: Double property detected in server configuration.";
}

const char *ServerPropertiesProcessor::UnknownErrorCodeException::what() const throw() {
	return "Error: webserv: unknown error page code in configuration file.";
}
