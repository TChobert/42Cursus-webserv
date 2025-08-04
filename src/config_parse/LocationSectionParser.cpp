#include "LocationSectionParser.hpp"
#include "ConfigParser.hpp"

LocationSectionParser::LocationSectionParser(void) {}

LocationSectionParser::~LocationSectionParser(void) {}

void LocationSectionParser::setupLocationDefaultMethods(parserContext *context) {

	std::vector<std::string> defaultMethods = {"GET", "POST", "DELETE"};

	defaultMethods.swap(context->currentConfig.locations[context->currentLocationName].allowedMethods);
}

void LocationSectionParser::setupLocationDefaultRoot(parserContext *context) {

	context->currentConfig.locations[context->currentLocationName].hasRoot = false;

	if (context->currentConfig.identity.hasRoot == true) {
		context->currentConfig.locations[context->currentLocationName].root = context->currentConfig.identity.root;
	} else
		throw RootlessLocationException();
}

void LocationSectionParser::ensureLocationConfigIsFull(parserContext *context) {

	if (context->seenLocationProperties.indexSeen == false)
		context->currentConfig.locations[context->currentLocationName].autoIndex = false;
	if (context->seenLocationProperties.rootSeen == false) {
		setupLocationDefaultRoot(context);
	}
	if (context->seenLocationProperties.maxBodySeen == false)
		context->currentConfig.locations[context->currentLocationName].clientMaxBodySize = DEFAULT_BODY_SIZE;
	if (context->seenLocationProperties.uploadAuthSeen == false)
		context->currentConfig.locations[context->currentLocationName].uploadEnabled = false;
	if (context->seenLocationProperties.returnSeen == false)
		context->currentConfig.locations[context->currentLocationName].hasRedir = false;
	if (context->seenLocationProperties.allowedMethodsSeen == false)
		setupLocationDefaultMethods(context);
}

void LocationSectionParser::extractCurrentProperty(const std::string& property, parserContext *context) {

	if (PropertyProcessor::IsSectionHeader(property) == true) {
		ensureLocationConfigIsFull(context);
		context->seenLocationProperties.resetFlags();
		context->state = HEADER_SECTION;
	}
	else {
		keyValue propertyKeyValue = PropertyProcessor::extractKeyValueFromProperty(property);

		LocationPropertiesProcessor::LocationProcessPtr processFunction = _propertiesProcessor.getLocationPropertyProcess(propertyKeyValue.key);
		(_propertiesProcessor.*processFunction)(propertyKeyValue.value, context);
	}
}

// EXCEPTIONS

const char *LocationSectionParser::RootlessLocationException::what() const throw() {
	return "Error: webserv: location without root or server root detected in configuration file.";
}
