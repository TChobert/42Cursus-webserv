#include "LocationSectionParser.hpp"
#include "ConfigParser.hpp"

LocationSectionParser::LocationSectionParser(void) {}

LocationSectionParser::~LocationSectionParser(void) {}

void LocationSectionParser::extractCurrentProperty(const std::string& property, parserContext *context) {

	if (PropertyProcessor::IsSectionHeader(property) == true) {
		//ensureLocationConfigIsFull(context);
		context->state = HEADER_SECTION;
	}
	else {
		keyValue propertyKeyValue = PropertyProcessor::extractKeyValueFromProperty(property);

		LocationPropertiesProcessor::LocationProcessPtr processFunction = _propertiesProcessor.getLocationPropertyProcess(propertyKeyValue.key);
		(_propertiesProcessor.*processFunction)(propertyKeyValue.value, context);
	}
}
