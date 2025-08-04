#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "keyValue.hpp"
#include "LocationPropertiesProcessor.hpp"

struct parserContext;

class LocationSectionParser {

	private:

	LocationPropertiesProcessor _propertiesProcessor;

	void ensureLocationConfigIsFull(parserContext *context);
	void setupLocationDefaultMethods(parserContext *context);
	void setupLocationDefaultRoot(parserContext *context);

	public:

	LocationSectionParser(void);
	~LocationSectionParser(void);

	void extractCurrentProperty(const std::string& property, parserContext *context);
};
