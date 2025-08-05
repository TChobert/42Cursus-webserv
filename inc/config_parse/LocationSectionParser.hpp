#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "keyValue.hpp"
#include "LocationPropertiesProcessor.hpp"
#include "HeaderSectionParser.hpp"

struct parserContext;

class LocationSectionParser {

	private:

	HeaderSectionParser _headerParser;
	LocationPropertiesProcessor _propertiesProcessor;

	// void ensureLocationConfigIsFull(parserContext *context);
	// void setupLocationDefaultMethods(parserContext *context);
	// void setupLocationDefaultRoot(parserContext *context);

	public:

	LocationSectionParser(void);
	~LocationSectionParser(void);

	void ensureLocationConfigIsFull(parserContext *context);
	void setupLocationDefaultMethods(parserContext *context);
	void setupLocationDefaultRoot(parserContext *context);
	void extractCurrentProperty(const std::string& property, parserContext *context);

	class RootlessLocationException : public std::exception {
		virtual const char * what() const throw();
	};
};
