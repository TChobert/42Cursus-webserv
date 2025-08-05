#include "ServerPropertiesProcessor.hpp"
#include "ConfigParser.hpp"

ServerPropertiesProcessor::ServerPropertiesProcessor(void) {}

ServerPropertiesProcessor::~ServerPropertiesProcessor(void) {}

const int ServerPropertiesProcessor::validErrorCodes[] = {400, 401, 403, 404, 405, 408, 413, 500, 501, 504};

const char * ServerPropertiesProcessor::forbiddenPaths[] = {
	"/etc", "/root", "/bin", "/sbin", "/dev", "/proc", "/sys", "/usr/bin"
};

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

void ServerPropertiesProcessor::ensureRootIsAllowed(const std::string& root) {

	for (size_t i = 0; i < sizeof(forbiddenPaths) / sizeof(forbiddenPaths[0]); ++i) {

		const std::string forbidden(forbiddenPaths[i]);

		if (root == forbidden || (root.size() > forbidden.size()
			&& root.compare(0, forbidden.size(), forbidden) == 0 && root[forbidden.size()] == '/')) {
				throw ForbiddenServerRootException();
			}
	}
}

void ServerPropertiesProcessor::ensureRootIsValid(const std::string& root) {

	struct stat pathStat;

	if (access(root.c_str(), F_OK | R_OK | X_OK) != 0) {
		throw InvalidServerRootException();
	}
	if (stat(root.c_str(), &pathStat) != 0 || !S_ISDIR(pathStat.st_mode))
		throw InvalidServerRootException();
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
	throw UnknownErrorCodeException();
	return (0);
}

void ServerPropertiesProcessor::ensureErrorPathIsValid(const std::string& pagePath) {

	if (pagePath.find("..") != std::string::npos || pagePath.empty()) {
		throw InvalidErrorPageException();
	}
	if (pagePath[0] == '/') {
		throw InvalidErrorPageException();
	}
	std::ifstream pageStream(pagePath.c_str());
	if (!pageStream.good()) {
		throw UnexistantErrorPageException();
	}
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
		throw DoubleErrorPathException();
	}
	if (isValueInMap(context->currentConfig.errorPagesCodes, pagePath)) {
		throw DoubleErrorPathException();
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
	if (propertyValue.empty() || propertyValue.find("..") != std::string::npos) {
		throw InvalidServerRootException();
	}
	ensureRootIsAllowed(propertyValue);
//	ensureRootIsValid(propertyValue);
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
	ensureErrorPathIsValid(pagePath);
	addErrorPageToErrorMap(context, codeValue, pagePath);
}

ServerPropertiesProcessor::ServerProcessPtr ServerPropertiesProcessor::getPropertyProcess(const std::string& directiveKey) {

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
			return &ServerPropertiesProcessor::processErrorPageProperty;
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
	return "Error: webserv: Invalid server root detected in configuration file.";
}

const char *ServerPropertiesProcessor::ForbiddenServerRootException::what() const throw() {
	return "Error: webserv: Forbidden server root directory detected in configuration file.";
}

const char *ServerPropertiesProcessor::InvalidErrorPageException::what() const throw() {
	return "Error: webserv: Invalid error_page path property detected. Must be at format: error_page=404 error/404.html. Absolute paths are not accepted.";
}

const char *ServerPropertiesProcessor::DoublePropertyException::what() const throw() {
	return "Error: webserv: Double property detected in server configuration.";
}

const char *ServerPropertiesProcessor::UnknownErrorCodeException::what() const throw() {
	return "Error: webserv: unknown error page code in configuration file.";
}

const char *ServerPropertiesProcessor::DoubleErrorPathException::what() const throw() {
	return "Error: webserv: duplicate error_page path property detected in configuration file.";
}

const char *ServerPropertiesProcessor::UnexistantErrorPageException::what() const throw() {
	return "Error: webserv: non existant error_page path detected in configuration file.";
}
