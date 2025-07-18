#include "ServerSectionParser.hpp"
#include "ConfigParser.hpp"

ServerSectionParser::ServerSectionParser(void) {}

ServerSectionParser::~ServerSectionParser(void) {}

bool ServerSectionParser::IsSectionHeader(const std::string& property) {

	return (property[0] == HEADER_START);
}

void ServerSectionParser::trimSpaces(std::string& toTrim) {

	size_t start = 0;
	while (isspace(static_cast<unsigned char>(toTrim[start]))) {
		++start;
	}
	size_t end = toTrim.size();
	while (isspace(static_cast<unsigned char>(toTrim[end - 1]))) {
		--end;
	}
	std::string trimmed = toTrim.substr(start, end -start);
	toTrim = trimmed;
}

void ServerSectionParser::splitProperty(keyValue *storage, const std::string& property, size_t delimiter) {

	storage->key = property.substr(0, (delimiter));
	storage->value = property.substr(delimiter + 1);
	trimSpaces(storage->key);
	trimSpaces(storage->value);
}

keyValue ServerSectionParser::extractKeyValueFromProperty(const std::string& property) {

	size_t delimiter = property.find("=");

	if (delimiter == std::string::npos) {
		delimiter = property.find(":");
		if (delimiter == std::string::npos) {
			throw InvalidPropertyDelimiter();
		}
	}
	if (delimiter == property.size()) {
		throw InvalidProperty();
	}
	keyValue propertyKeyAndValue;
	splitProperty(&propertyKeyAndValue, property, delimiter);
	return (propertyKeyAndValue);
}

void ServerSectionParser::extractCurrentDirective(const std::string& property, parserContext *context) {

	keyValue propertyKeyValue = extractKeyValueFromProperty(property);

	ServerDirectivesProcessor::ProcessPtr processFunction = _directivesProcessor.getDirectiveProcess(propertyKeyValue.key);
	(_directivesProcessor.*processFunction)(propertyKeyValue.value, context);
}

//EXCEPTIONS

const char* ServerSectionParser::InvalidPropertyDelimiter::what() const throw() {
	return "Invalid property delimiter";
}

const char* ServerSectionParser::InvalidProperty::what() const throw() {
	return "Invalid property";
}
