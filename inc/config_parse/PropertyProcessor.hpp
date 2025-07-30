#pragma once

#include <string>
#include <iostream>
#include <exception>

#include "keyValue.hpp"

class PropertyProcessor {

	public:

	PropertyProcessor(void);
	~PropertyProcessor(void);

	static keyValue extractKeyValueFromProperty(const std::string& property);
	static void splitProperty(keyValue *storage, const std::string& property, size_t delimiter);
	static void trimSpaces(std::string& toTrim);

	class InvalidPropertyDelimiter : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidProperty : public std::exception {
		virtual const char *what() const throw();
	};
};
