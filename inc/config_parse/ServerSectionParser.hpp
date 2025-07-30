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
	void ensureServerConfigIsFull(parserContext *context);

	public:

	ServerSectionParser(void);
	~ServerSectionParser(void);

	void extractCurrentProperty(const std::string& property, parserContext *context);

	class InvalidPropertyDelimiter : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidProperty : public std::exception {
		virtual const char *what() const throw();
	};
	class UncompleteServerConfigException : public std::exception {
		virtual const char *what() const throw();
	};
};
