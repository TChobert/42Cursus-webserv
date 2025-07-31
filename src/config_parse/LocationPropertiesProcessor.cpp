#include "LocationPropertiesProcessor.hpp"

LocationPropertiesProcessor::LocationPropertiesProcessor(void) {}

LocationPropertiesProcessor::~LocationPropertiesProcessor(void) {}

LocationPropertiesProcessor::keyType LocationPropertiesProcessor::getKeyType(const std::string& key) {

	if (key == "root")
		return (ROOT);
	else if (key == "allowed_methods")
		return (ALLOWED_METHODS);
	else if (key == "upload_enabled")
		return (UPLOAD_AUT);
	else if (key == "upload_directory")
		return (UPLOAD_STORE);
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

LocationPropertiesProcessor::LocationProcessPtr LocationPropertiesProcessor::getLocationPropertyProcess(const std::string& key) {

	keyType type = getKeyType(key);

	if (type == UNKNOWN) {
	//	throw InvalidLocationPropertyException();
	}
	switch (type) {
		case ROOT :
			//return &LocationPropertiesProcessor::fetchLocationRootProperty;
		case ALLOWED_METHODS :
			//return &LocationPropertiesProcessor::fetchMethodsProperty;
	}
}