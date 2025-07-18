#include "ServerSectionParser.hpp"

ServerSectionParser::ServerSectionParser(void) {}

ServerSectionParser::~ServerSectionParser(void) {}

bool ServerSectionParser::IsSectionHeader(const std::string& currentDirective) {

	return (currentDirective[0] == HEADER_START);
}

