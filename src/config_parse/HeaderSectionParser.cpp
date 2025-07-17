#include "HeaderSectionParser.hpp"

HeaderSectionParser::HeaderSectionParser(void) {}

HeaderSectionParser::~HeaderSectionParser(void) {}

void HeaderSectionParser::ensureHeaderIsEnclosed(const std::string& header) {

	size_t headerClose = header.find(HEADER_CLOSE);

	if (header[0] != HEADER_OPEN || headerClose == std::string::npos)
		throw InvalidHeaderException();

	for (size_t i = headerClose; i < header.size(); ++i) {
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

HeaderSectionParser::headerType HeaderSectionParser::getHeaderType(const std::string& header) {

	if (header == "SERVER") 
			return (SERVER);
	else if (header.find("LOCATION") == 0) {
		return (LOCATION);
	}
	else
		throw InvalidHeaderException();
}

void HeaderSectionParser::handleCurrentHeader(const std::string& header, parserState *state) {

	ensureHeaderIsEnclosed(header);
	std::string trimmedHeader = trimHeader(header);
	headerType type = getHeaderType(trimmedHeader);

	if (type == SERVER && state->isInServerScope == true) {
		state->isConfigComplete = true;
		state->state = SERVER_SECTION;
	}
	else if (type == LOCATION) {
		if (state->isInServerScope == false) {
			throw ServerlessSectionException();
		}
		state->state = LOCATION_SECTION;
	}
}
