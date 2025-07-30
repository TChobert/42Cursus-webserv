#include "ServerSectionParser.hpp"
#include "ConfigParser.hpp"

ServerSectionParser::ServerSectionParser(void) {}

ServerSectionParser::~ServerSectionParser(void) {}

void ServerSectionParser::ensureServerConfigIsFull(parserContext *context) {

	if (context->seenServerProperties.hostSeen == false || context->seenServerProperties.portSeen == false) {
		throw UncompleteServerConfigException();
	}
}

void ServerSectionParser::extractCurrentProperty(const std::string& property, parserContext *context) {

	if (PropertyProcessor::IsSectionHeader(property) == true) {
		ensureServerConfigIsFull(context);
		context->state = HEADER_SECTION;
	}
	else {
		keyValue propertyKeyValue = PropertyProcessor::extractKeyValueFromProperty(property);

		ServerPropertiesProcessor::ProcessPtr processFunction = _propertiesProcessor.getPropertyProcess(propertyKeyValue.key);
		(_propertiesProcessor.*processFunction)(propertyKeyValue.value, context);
	}
}

//EXCEPTIONS

const char* ServerSectionParser::UncompleteServerConfigException::what() const throw() {
	return "Error: webserv: Uncomplete server configuration detected in configuration file.\nA server configuration must at least contains a root and a host.";
}
