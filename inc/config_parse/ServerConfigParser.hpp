#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "ConfigParser.hpp"

class ServerConfigParser {

	private:

	enum keyType{PORT, HOST, NAME, ROOT};
	bool IsSectionHeader(const std::string& currentLine);
	keyType getKeyType(const std::string& key);

	public:

	void extractCurrentDirective(const std::string& directive, parserState *state);
};
