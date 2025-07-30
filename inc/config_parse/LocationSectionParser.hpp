#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "keyValue.hpp"

struct parserContext;

class LocationSectionParser {

	private:

	static const char HEADER_START = '[';
	//enum keyType{PORT, HOST, NAME, ROOT};
	bool IsSectionHeader(const std::string& property);
	//keyType getKeyType(const std::string& key);

	public:

	LocationSectionParser(void);
	~LocationSectionParser(void);

	void extractCurrentProperty(const std::string& property, parserContext *context);
};
