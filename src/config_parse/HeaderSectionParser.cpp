#include "HeaderSectionParser.hpp"

HeaderSectionParser::HeaderSectionParser(void) {}

HeaderSectionParser::~HeaderSectionParser(void) {}

size_t HeaderSectionParser::getHeaderLastChar(const std::string& header) {

	size_t lastChar = header.size();

	while (isspace(header[lastChar])) {
		--lastChar;
	}
	return (lastChar);
}

HeaderSectionParser::headerType HeaderSectionParser::getHeaderType(const std::string& header) {

	size_t lastChar = getHeaderLastChar(header);

	if (header[0] != '[' || header[lastChar] != ']') {
		throw (InvalidHeaderException());
	}
}

void HeaderSectionParser::handleCurrentHeader(const std::string& header, parserState *state) {

	headerType type = getHeaderType(header);
}
