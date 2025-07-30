#pragma once

#include <string>
#include <iostream>
#include <exception>

#include "keyValue.hpp"

class PropertyProcessor {

	private:

	PropertyProcessor(void);
	~PropertyProcessor(void);
	static const char HEADER_START = '[';

	public:

	static keyValue extractKeyValueFromProperty(const std::string& property);
	static void splitProperty(keyValue& storage, const std::string& property, size_t delimiter);
	static void trimSpaces(std::string& toTrim);
	static bool IsSectionHeader(const std::string& property);

	class InvalidPropertyDelimiter : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidProperty : public std::exception {
		virtual const char *what() const throw();
	};
};
