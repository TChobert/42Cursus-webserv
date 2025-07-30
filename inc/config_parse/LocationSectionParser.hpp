#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "keyValue.hpp"

struct parserContext;

class LocationSectionParser {

	private:

	//enum keyType{PORT, HOST, NAME, ROOT};
	//keyType getKeyType(const std::string& key);

	public:

	LocationSectionParser(void);
	~LocationSectionParser(void);

	void extractCurrentProperty(const std::string& property, parserContext *context);
};
