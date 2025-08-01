#include "LocationPropertiesProcessor.hpp"
#include "ConfigParser.hpp"

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
	else if (key == "autoindex")
		return (AUTOINDEX);
	else if (key == "cgi")
		return (CGI);
	else if (key == "return")
		return (RETURN);
	else if (key == "client_max_body_size")
		return (BODY_SIZE);
	else if (key == "error_page")
		return (ERROR_PAGE);
	else
		return (UNKNOWN);
}

bool LocationPropertiesProcessor::isValidMethod(const std::string& method) const {

	return (method == "GET" || method == "POST" || method == "DELETE");
}

void LocationPropertiesProcessor::processMethodsProperty(const std::string& property, parserContext *context) {

	std::vector<std::string> methods;

	if (context->seenLocationProperties.allowedMethodsSeen == true)
		throw DoubleLocationPropertyException();
	if (property.empty())
		throw EmptyLocationPropertyException();

	methods = split(property, SPACE);
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
		if (isValidMethod(*it)) {
			context->currentConfig.locations[context->currentLocationName].allowedMethods.push_back(*it);
		}
		else
			throw InvalidMethodException();
	}
	context->seenLocationProperties.allowedMethodsSeen = true;
}

void LocationPropertiesProcessor::fetchUploadAuthorisation(const std::string& property, parserContext* context) {

	if (context->seenLocationProperties.uploadAuthSeen == true)
		throw DoubleLocationPropertyException();

	if (property == "true")
		context->currentConfig.locations[context->currentLocationName].uploadEnabled = true;
	else if (property == "false")
		context->currentConfig.locations[context->currentLocationName].uploadEnabled = false;
	else
		throw InvalidUploadAuthException();

	context->seenLocationProperties.uploadAuthSeen = true;
}

void LocationPropertiesProcessor::processUploadDirProperty(const std::string& property, parserContext *context) {

	if (context->seenLocationProperties.uploadLocSeen == true)
		throw DoubleLocationPropertyException();
	if (property.empty())
		throw EmptyLocationPropertyException();

		//suite
}

void LocationPropertiesProcessor::fetchAutoIndex(const std::string& property, parserContext *context) {

	if (context->seenLocationProperties.autoIndexSeen == true)
		throw DoubleLocationPropertyException();

	if (property == "on")
		context->currentConfig.locations[context->currentLocationName].autoIndex = true;
	else if (property == "off")
		context->currentConfig.locations[context->currentLocationName].autoIndex = false;
	else
		throw InvalidAutoIndexException();

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

void LocationPropertiesProcessor::processCgiProperty(const std::string& property, parserContext *context) {

	// creer une map, la swap.
}

LocationPropertiesProcessor::LocationProcessPtr LocationPropertiesProcessor::getLocationPropertyProcess(const std::string& key) {

	keyType type = getKeyType(key);

	if (type == UNKNOWN) {
		throw InvalidLocationPropertyException();
	}
	switch (type) {
	// 	case ROOT :
	// 		//return &LocationPropertiesProcessor::processLocationRootProperty;
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
	}
	return (NULL);
}

// EXCEPTIONS

const char *LocationPropertiesProcessor::InvalidLocationPropertyException::what() const throw() {
	 return ("Error: webserv: Invalid location property detected in configuration file");
}

const char *LocationPropertiesProcessor::EmptyLocationPropertyException::what() const throw() {
	 return ("Error: webserv: Empty location property detected in configuration file");
}

const char *LocationPropertiesProcessor::InvalidUploadAuthException::what() const throw() {
	 return ("Error: webserv: Invalid upload authorisation property detected in configuration file. Must be at format: upload_enabled=true | false");
}

const char *LocationPropertiesProcessor::DoubleLocationPropertyException::what() const throw() {
	 return ("Error: webserv: Double property detected in a location section of configuration file.");
}

const char *LocationPropertiesProcessor::InvalidMethodException::what() const throw() {
	 return ("Error: webserv: invalid methods authorisation list in a location section of configuration file. Accepeted: GET POST DELETE.");
}

const char *LocationPropertiesProcessor::InvalidAutoIndexException::what() const throw() {
	 return ("Error: webserv: invalid auto index property detected in a location section. Must be on | off only.");
}
