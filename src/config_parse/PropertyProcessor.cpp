#include "PropertyProcessor.hpp"

PropertyProcessor::PropertyProcessor(void) {}

PropertyProcessor::~PropertyProcessor(void) {}

void PropertyProcessor::trimSpaces(std::string& toTrim) {

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

void PropertyProcessor::splitProperty(keyValue *storage, const std::string& property, size_t delimiter) {

	storage->key = property.substr(0, (delimiter));
	storage->value = property.substr(delimiter + 1);
	trimSpaces(storage->key);
	trimSpaces(storage->value);
}

keyValue PropertyProcessor::extractKeyValueFromProperty(const std::string& property) {

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

// EXCEPTIONS

const char* PropertyProcessor::InvalidPropertyDelimiter::what() const throw() {
	return "Error: webserv: Invalid property delimiter detected in configuration file.";
}

const char* PropertyProcessor::InvalidProperty::what() const throw() {
	return "Error: webserv: Invalid property detected in configuration file.";
}
