#pragma once

#include <string>
#include <iostream>
#include <exception>
#include <sstream>

#include "ConfigParser.hpp"

class HeaderSectionParser {

	private:

	enum headerType {SERVER, LOCATION};
	size_t getHeaderLastChar(const std::string& header);
	headerType getHeaderType(const std::string& header);

	public:

	HeaderSectionParser(void);
	~HeaderSectionParser(void);

	void handleCurrentHeader(const std::string& header, parserState *state);

	class InvalidHeaderException : public std::exception {
		virtual const char *what() const throw();
	};
};
