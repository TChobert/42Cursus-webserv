#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "PropertyProcessor.hpp"
#include "ServerPropertiesProcessor.hpp"
#include "keyValue.hpp"

class ServerSectionParser {

	private:

	ServerPropertiesProcessor _propertiesProcessor;

	bool IsSectionHeader(const std::string& property);
	keyValue extractKeyValueFromProperty(const std::string& directive);
	void ensureServerConfigIsFull(parserContext *context);

	public:

	ServerSectionParser(void);
	~ServerSectionParser(void);

	void extractCurrentProperty(const std::string& property, parserContext *context);

	class UncompleteServerConfigException : public std::exception {
		virtual const char *what() const throw();
	};
};
