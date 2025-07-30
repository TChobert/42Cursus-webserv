#include "LocationSectionParser.hpp"
#include "ConfigParser.hpp"

LocationSectionParser::LocationSectionParser(void) {}

LocationSectionParser::~LocationSectionParser(void) {}

bool LocationSectionParser::IsSectionHeader(const std::string& property) {

	return (property[0] == HEADER_START);
}

void LocationSectionParser::extractCurrentProperty(const std::string& property, parserContext *context) {

	if (IsSectionHeader(property) == true) {
		//ensureLocationConfigIsFull(context);
		context->state = HEADER_SECTION;
	}
	else {
		keyValue propertyKeyValue = extractKeyValueFromProperty(property);
	}
}
