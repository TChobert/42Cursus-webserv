#include "HeaderSectionParser.hpp"
#include "ConfigParser.hpp"

const char* const HeaderSectionParser::LOCATION_KEYWORD = "LOCATION";
const char* const HeaderSectionParser::SERVER_KEYWORD = "SERVER";

HeaderSectionParser::HeaderSectionParser(void) {}

HeaderSectionParser::~HeaderSectionParser(void) {}

void HeaderSectionParser::ensureHeaderIsEnclosed(const std::string& header) {

	size_t headerClose = header.find(HEADER_CLOSE);

	if (header[0] != HEADER_OPEN || headerClose == std::string::npos)
		throw InvalidHeaderException();

	for (size_t i = (headerClose + 1); i < header.size(); ++i) {
		if (!isspace(static_cast<unsigned char>(header[i]))) {
			throw InvalidHeaderException();
		}
	}
}

std::string HeaderSectionParser::trimHeader(const std::string& header) {

	size_t headerClose = header.find(HEADER_CLOSE);

	std::string trimmedHeader = header.substr(1, (headerClose -1));
	size_t start = 0;
	while (start < trimmedHeader.size() && isspace(static_cast<unsigned char>(trimmedHeader[start]))) {
		++start;
	}
	size_t end = trimmedHeader.size();
	while (end > 0 && isspace(static_cast<unsigned char>(trimmedHeader[end - 1]))) {
		--end;
	}
	return (trimmedHeader.substr(start, end - start));
}

void HeaderSectionParser::getLocationName(const std::string& header, parserContext *context) {

	std::string locationName = header.substr(std::strlen(LOCATION_KEYWORD));

	size_t i = 0;
	while (i < locationName.size() && isspace(static_cast<unsigned char>(locationName[i]))) {
		++i;
	}
	locationName = locationName.substr(i);
	size_t prefixPosition = locationName.find('/');
	if (prefixPosition == std::string::npos || prefixPosition != 0) {
		throw InvalidHeaderException();
	}
	locationConfig config;
	context->currentConfig.locations[locationName] = config;
	context->currentLocationName = locationName;
}

HeaderSectionParser::headerType HeaderSectionParser::getHeaderType(const std::string& header) {

	if (header == SERVER_KEYWORD) 
			return (SERVER);
	else if (header.find(LOCATION_KEYWORD) == 0) {
		return (LOCATION);
	}
	else
		throw InvalidHeaderException();
}

void HeaderSectionParser::handleCurrentHeader(const std::string& header, parserContext *context) {

	ensureHeaderIsEnclosed(header);
	std::string trimmedHeader = trimHeader(header);
	headerType type = getHeaderType(trimmedHeader);

	if (type == SERVER && context->isInServerScope == true) {
		context->isConfigComplete = true;
		context->state = SERVER_SECTION;
	}
	else if (type == SERVER && context->isInServerScope == false) {
		context->isInServerScope = true;
		context->state = SERVER_SECTION;
	}
	else if (type == LOCATION) {
		if (context->isInServerScope == false) {
			throw ServerlessSectionException();
		}
		getLocationName(trimmedHeader, context);
		context->state = LOCATION_SECTION;
	}
}

//EXCEPTIONS

const char* HeaderSectionParser::InvalidHeaderException::what() const throw() {
	return "Invalid header format.";
}

const char* HeaderSectionParser::ServerlessSectionException::what() const throw() {
	return "LOCATION section found before any SERVER section.";
}

const char* HeaderSectionParser::InvalidLocationName::what() const throw() {
	return "Invalid location name.";
}
