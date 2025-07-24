#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

struct parserContext;

class LocationSectionParser {

	private:

	//enum keyType{PORT, HOST, NAME, ROOT};
	bool IsSectionHeader(const std::string& currentLine);
	//keyType getKeyType(const std::string& key);

	public:

	LocationSectionParser(void);
	~LocationSectionParser(void);

	void extractCurrentProperty(const std::string& directive, parserContext *context);
};
