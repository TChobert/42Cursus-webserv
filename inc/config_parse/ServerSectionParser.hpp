#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>

#include "ServerPropertiesProcessor.hpp"

struct parserContext;

struct keyValue {
	std::string key;
	std::string value;
};

class ServerSectionParser {

	private:

	static const char HEADER_START = '[';
	ServerPropertiesProcessor _propertiesProcessor;

	bool IsSectionHeader(const std::string& property);
	keyValue extractKeyValueFromProperty(const std::string& directive);
	void splitProperty(keyValue *storage, const std::string& property, size_t delimiter);
	void trimSpaces(std::string& toTrim);

	public:

	ServerSectionParser(void);
	~ServerSectionParser(void);

	void extractCurrentProperty(const std::string& property, parserContext *context);

	class InvalidPropertyDelimiter : public std::exception {
	public:
		virtual const char *what() const throw();
	};
	class InvalidProperty : public std::exception {
	public:
		virtual const char *what() const throw();
	};
};
