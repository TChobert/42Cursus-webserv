#include "LocationPropertiesProcessor.hpp"
#include "ConfigParser.hpp"

const char * LocationPropertiesProcessor::cgiExtensions[] = {
	".py", ".sh", ".pl"
};

const char * LocationPropertiesProcessor::forbiddenPaths[] = {
	"/etc", "/root", "/bin", "/sbin", "/dev", "/proc", "/sys", "/usr/bin"
};

const std::string LocationPropertiesProcessor::httpPrefix = "http://";
const std::string LocationPropertiesProcessor::httpsPrefix = "https://";

LocationPropertiesProcessor::LocationPropertiesProcessor(void) {}

LocationPropertiesProcessor::~LocationPropertiesProcessor(void) {}

LocationPropertiesProcessor::keyType LocationPropertiesProcessor::getKeyType(const std::string& key) {

	if (key == "root")
		return (ROOT);
	else if (key == "allowed_methods")
		return (ALLOWED_METHODS);
	else if (key == "upload_enabled")
		return (UPLOAD_AUTH);
	else if (key == "upload_directory")
		return (UPLOAD_DIR);
	else if (key == "auto_index")
		return (AUTOINDEX);
	else if (key == "index")
		return (INDEX);
	else if (key == "cgi")
		return (CGI);
	else if (key == "return")
		return (RETURN);
	else if (key == "client_max_body_size")
		return (BODY_SIZE);
	else
		return (UNKNOWN);
}

void LocationPropertiesProcessor::ensureRootIsAllowed(const std::string& root) const {

	const size_t forbiddenCount = sizeof(forbiddenPaths) / sizeof(forbiddenPaths[0]);

	for (size_t i = 0; i < forbiddenCount; ++i) {
		const std::string forbidden(forbiddenPaths[i]);

		if (root == forbidden || (root.size() > forbidden.size()
			&& root.compare(0, forbidden.size(), forbidden) == 0 && root[forbidden.size()] == '/')) {
				throw ForbiddenLocationRootException();
		}
	}
}

void LocationPropertiesProcessor::ensureUploadDirIsValid(const std::string& dir) const {

	struct stat pathStat;

	if (access(dir.c_str(), F_OK | R_OK | X_OK) != 0) {
		std::cerr << dir << " : ";
		throw InvalidUploadDirException();
	}
	if (stat(dir.c_str(), &pathStat) != 0 || !S_ISDIR(pathStat.st_mode)) {
		std::cerr << dir << " : ";
		throw InvalidUploadDirException();
	}
}

void LocationPropertiesProcessor::ensureUploadDirIsAllowed(const std::string& dir) const {

	const size_t forbiddenCount = sizeof(forbiddenPaths) / sizeof(forbiddenPaths[0]);
	for (size_t i = 0; i < forbiddenCount; ++i) {

		const std::string forbidden(forbiddenPaths[i]);

		if (dir == forbidden || (dir.size() > forbidden.size()
			&& dir.compare(0, forbidden.size(), forbidden) == 0 && dir[forbidden.size()] == '/')) {
				std::cerr << dir << " : ";
				throw ForbiddenUploadDirException();
		}
	}
}

void LocationPropertiesProcessor::processLocationRootProperty(const std::string& property, parserContext *context) {

	if (context->seenLocationProperties.rootSeen)
		throw DoubleLocationPropertyException();
	if (property.empty() || property.find("..") != std::string::npos) {
		std::cerr << property << " : ";
		throw ForbiddenLocationRootException();
	}
	ensureRootIsAllowed(property);
	context->currentConfig.locations[context->currentLocationName].root = property;
	context->currentConfig.locations[context->currentLocationName].hasRoot = true;
	context->seenLocationProperties.rootSeen = true;
}

bool LocationPropertiesProcessor::isValidMethod(const std::string& method) const {

	return (method == "GET" || method == "POST" || method == "DELETE");
}

void LocationPropertiesProcessor::processMethodsProperty(const std::string& property, parserContext *context) {

	std::vector<std::string> methods;

	if (context->seenLocationProperties.allowedMethodsSeen) {
		std::cerr << property << " : ";
		throw DoubleLocationPropertyException();
	}
	if (property.empty())
		throw EmptyLocationPropertyException();

	methods = split(property, SPACE);
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
		if (isValidMethod(*it)) {
			context->currentConfig.locations[context->currentLocationName].allowedMethods.push_back(*it);
		}
		else {
			std::cerr << property << " : ";
			throw InvalidMethodException();
		}
	}
	context->seenLocationProperties.allowedMethodsSeen = true;
}

void LocationPropertiesProcessor::fetchUploadAuthorisation(const std::string& property, parserContext* context) {

	if (context->seenLocationProperties.uploadAuthSeen)
		throw DoubleLocationPropertyException();

	if (property == "true")
		context->currentConfig.locations[context->currentLocationName].uploadEnabled = true;
	else if (property == "false")
		context->currentConfig.locations[context->currentLocationName].uploadEnabled = false;
	else {
		std::cerr << property << " : ";
		throw InvalidUploadAuthException();
	}
	context->seenLocationProperties.uploadAuthSeen = true;
}

void LocationPropertiesProcessor::processUploadDirProperty(const std::string& property, parserContext *context) {

	if (context->seenLocationProperties.uploadDirSeen)
		throw DoubleLocationPropertyException();
	if (property.empty() || property.find("..") != std::string::npos) { //|| property[0] != '/') {
		std::cerr << property << " : ";
		throw InvalidUploadDirException();
	}

	ensureUploadDirIsAllowed(property);
	ensureUploadDirIsValid(property);

	context->currentConfig.locations[context->currentLocationName].uploadStore = property;
	context->seenLocationProperties.uploadDirSeen = true;
}

void LocationPropertiesProcessor::fetchAutoIndex(const std::string& property, parserContext *context) {

	if (context->seenLocationProperties.autoIndexSeen) {
		std::cerr << property << " : ";
		throw DoubleLocationPropertyException();
	}

	if (property == "on")
		context->currentConfig.locations[context->currentLocationName].autoIndex = true;
	else if (property == "off")
		context->currentConfig.locations[context->currentLocationName].autoIndex = false;
	else {
		std::cerr << property << " : ";
		throw InvalidAutoIndexException();
	}
	context->seenLocationProperties.autoIndexSeen = true;
}

void LocationPropertiesProcessor::processIndexProperty(const std::string& property, parserContext* context) {

	if (property.empty())
		throw EmptyLocationPropertyException();

	std::vector<std::string> indexes = split(property, SPACE);
	for (std::vector<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); ++it) {
		if (!it->empty()) {
			context->currentConfig.locations[context->currentLocationName].indexFiles.push_back(*it);
		}
	}
	context->seenLocationProperties.indexSeen = true;
}

bool LocationPropertiesProcessor::isValidCgiHandler(const std::string& handler) const {

	if (handler[0] != '/')
		return (false);
	if (handler.find("..") != std::string::npos)
		return (false);

	struct stat st;
	if (stat(handler.c_str(), &st) !=0) {
		return (false);
	}
	return (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR));
}

bool LocationPropertiesProcessor::isValidCgiExtension(const std::string& extension) const {

	const size_t numExtensions = sizeof(cgiExtensions) / sizeof(cgiExtensions[0]);

	for (size_t i = 0; i < numExtensions; ++i) {
		if (extension == cgiExtensions[i])
			return (true);
	}
	return (false);
}

void LocationPropertiesProcessor::getCgiExtensionAndHandler(std::map<std::string, std::string>& cgiRules, const std::string& cgiRule) {

	size_t delimiter = cgiRule.find_first_of(":");
	if (delimiter == std::string::npos || delimiter == 0 || delimiter == cgiRule.size() - 1)
		throw InvalidCgiException();

	std::string extension = cgiRule.substr(0, delimiter);
	std::string handler = cgiRule.substr(delimiter + 1);

	if (!isValidCgiExtension(extension) || !isValidCgiHandler(handler)) {
		throw InvalidCgiException();
	}
	if (cgiRules.count(extension) == 0) {
		cgiRules[extension] = handler;
	} else
		throw InvalidCgiException();
}

void LocationPropertiesProcessor::processCgiProperty(const std::string& property, parserContext *context) {

	if (property.empty())
		throw EmptyLocationPropertyException();
	if (context->seenLocationProperties.cgiSeen == true) {
		std::cerr << property << " : ";
		throw DoubleLocationPropertyException();
	}

	std::map<std::string, std::string>cgiExtensionHandlers;

	std::vector<std::string> cgiRules = split(property, SPACE);
	for (std::vector<std::string>::iterator it = cgiRules.begin(); it != cgiRules.end(); ++it) {

		getCgiExtensionAndHandler(cgiExtensionHandlers, *it);
	}
	context->currentConfig.locations[context->currentLocationName].cgiHandlers = cgiExtensionHandlers;
	context->seenLocationProperties.cgiSeen = true;
}

statusCode LocationPropertiesProcessor::getReturnCode(const std::string& codeStr) const {

	char * endPtr;
	long code = std::strtol(codeStr.c_str(), &endPtr, 10);
	if (*endPtr != '\0')
		throw InvalidLocationReturnException();

	switch (code) {
		case 301 :
			return MOVED_PERMANENTLY;
		case 302 :
			return FOUND;
		default :
			throw InvalidLocationReturnException();
	}
}

bool LocationPropertiesProcessor::isValidUrl(const std::string& url) const {

	if (url.compare(0, httpPrefix.size(), httpPrefix) == 0)
		return (url.size() > httpPrefix.size());
	if (url.compare(0, httpsPrefix.size(), httpsPrefix) == 0)
		return (url.size() > httpsPrefix.size());
	return (false);
}

void LocationPropertiesProcessor::fetchLocationReturnInfo(const std::string& property, parserContext *context) {

	if (property.empty())
		throw EmptyLocationPropertyException();

	if (context->seenLocationProperties.returnSeen == true) {
		std::cerr << property << " : ";
		throw DoubleLocationPropertyException();
	}
	std::vector<std::string>codeAndURL = split(property, SPACE);
	if (codeAndURL.size() == 2 && isValidUrl(codeAndURL.at(1))) {

		statusCode code = getReturnCode(codeAndURL.at(0));

		context->currentConfig.locations[context->currentLocationName].hasRedir = true;
		context->currentConfig.locations[context->currentLocationName].redirCode = code;
		context->currentConfig.locations[context->currentLocationName].redirURL = codeAndURL.at(1);
		context->seenLocationProperties.returnSeen = true;
	} else {
		std::cerr << property << " : ";
		throw InvalidLocationReturnException();
	}
}

size_t	LocationPropertiesProcessor::getMaxBodyValueByUnit(long value, const std::string& unit) {

	if (unit == "K" || unit == "k")
		return (static_cast<size_t>(value * 1024));
	if (unit == "M" || unit == "m")
		return (static_cast<size_t>(value * 1024 * 1024));
	if (unit == "G" || unit == "g")
		return (static_cast<size_t>(value * 1024 * 1024 * 1024));
	throw InvalidBodySizeException();
}

void LocationPropertiesProcessor::fetchMaxBodySize(const std::string& property, parserContext *context) {

	if (context->seenLocationProperties.maxBodySeen) {
		std::cerr << property << " : ";
		throw DoubleLocationPropertyException();
	}

	size_t i = 0;
	while (i < property.size() && std::isdigit(property[i])) {
		++i;
	}
	if (i == 0) {
		std::cerr << property << " : ";
		throw InvalidBodySizeException();
	}

	long value = std::strtol(property.substr(0, i).c_str(), NULL, 10);
	if (value < 0 || value > MAX_BODY_VALUE_ALLOWED) {
		std::cerr << property << " : ";
		throw InvalidBodySizeException();
	}

	std::string unit = property.substr(i);
	size_t maxBodySize;
	if (unit.empty() ) {
		maxBodySize = static_cast<size_t>(value);
	} else {
		maxBodySize = getMaxBodyValueByUnit(value, unit);
	}
	context->currentConfig.locations[context->currentLocationName].clientMaxBodySize = maxBodySize;
	context->seenLocationProperties.maxBodySeen = true;
}

LocationPropertiesProcessor::LocationProcessPtr LocationPropertiesProcessor::getLocationPropertyProcess(const std::string& key) {

	keyType type = getKeyType(key);

	if (type == UNKNOWN) {
		std::cerr << key << " : ";
		throw InvalidLocationPropertyException();
	}
	switch (type) {
	 	case ROOT :
	 		return &LocationPropertiesProcessor::processLocationRootProperty;
		case ALLOWED_METHODS :
	 		return &LocationPropertiesProcessor::processMethodsProperty;
	 	case UPLOAD_AUTH :
	 		return &LocationPropertiesProcessor::fetchUploadAuthorisation;
	 	case UPLOAD_DIR :
	 		return &LocationPropertiesProcessor::processUploadDirProperty;
	 	case AUTOINDEX :
			return &LocationPropertiesProcessor::fetchAutoIndex;
		case INDEX :
	 		return &LocationPropertiesProcessor::processIndexProperty;
		case CGI :
			return &LocationPropertiesProcessor::processCgiProperty;
		case RETURN :
			return &LocationPropertiesProcessor::fetchLocationReturnInfo;
		case BODY_SIZE :
			return &LocationPropertiesProcessor::fetchMaxBodySize;
		case UNKNOWN :
			std::cerr << key << " : ";
			throw InvalidLocationPropertyException();
	}
	std::cerr << key << " : ";
	throw InvalidLocationPropertyException();
}

// EXCEPTIONS

const char *LocationPropertiesProcessor::InvalidLocationPropertyException::what() const throw() {
	 return ("Invalid location property detected in configuration file");
}

const char *LocationPropertiesProcessor::EmptyLocationPropertyException::what() const throw() {
	 return ("Error: webserv: Empty location property detected in configuration file");
}

const char *LocationPropertiesProcessor::InvalidUploadAuthException::what() const throw() {
	 return ("Invalid upload authorisation property detected in configuration file. Must be at format: upload_enabled=true | false");
}

const char *LocationPropertiesProcessor::DoubleLocationPropertyException::what() const throw() {
	 return ("Double property detected in a location section of configuration file.");
}

const char *LocationPropertiesProcessor::InvalidMethodException::what() const throw() {
	 return ("Invalid methods authorisation list in a location section of configuration file. Accepted: GET POST DELETE.");
}

const char *LocationPropertiesProcessor::InvalidAutoIndexException::what() const throw() {
	 return ("Invalid auto index property detected in a location section. Must be on | off only.");
}

const char *LocationPropertiesProcessor::InvalidCgiException::what() const throw() {
	 return ("Error: webserv: invalid auto cgi property detected in a location section. Must be at format: cgi= .py: abs_path/to/py/handler .pl: abs_path/to/.pl/handler.");
}

const char *LocationPropertiesProcessor::ForbiddenLocationRootException::what() const throw() {
	 return ("Forbidden location root directory detected in configuration file.");
}

const char *LocationPropertiesProcessor::InvalidUploadDirException::what() const throw() {
	 return ("Invalid upload directory detected in configuration file.");
}

const char *LocationPropertiesProcessor::ForbiddenUploadDirException::what() const throw() {
	 return ("Forbidden upload directory detected in configuration file.");
}

const char *LocationPropertiesProcessor::InvalidLocationReturnException::what() const throw() {
	 return ("Invalid return detected in a location section from configuration file."); // codes listing ?
}

const char *LocationPropertiesProcessor::InvalidBodySizeException::what() const throw() {
	 return ("Invalid client_max_body_size detected in a location section from configuration file.");
}
