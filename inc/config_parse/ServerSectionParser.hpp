#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "ConfigParser.hpp"
#include "ServerDirectivesProcessor.hpp"

class ServerSectionParser {

	private:

	static const char HEADER_START = '[';
	bool IsSectionHeader(const std::string& currentDirective);
	ServerDirectivesProcessor _directivesPorcessor;

	public:

	ServerSectionParser(void);
	~ServerSectionParser(void);

	void extractCurrentDirective(const std::string& directive, parserContext *context);
};
