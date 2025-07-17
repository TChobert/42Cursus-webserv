#pragma once

#include <string>
#include <iostream>
#include <exception>
#include <sstream>

#include "ConfigParser.hpp"

class HeaderSectionParser {

	private:

	static const char HEADER_OPEN = '[';
	static const char HEADER_CLOSE = ']';
	enum headerType {SERVER, LOCATION};

	void ensureHeaderIsEnclosed(const std::string& header);
	std::string trimHeader(const std::string& header);
	headerType getHeaderType(const std::string& header);

	public:

	HeaderSectionParser(void);
	~HeaderSectionParser(void);

	void handleCurrentHeader(const std::string& header, parserState *state);

	class InvalidHeaderException : public std::exception {
		virtual const char *what() const throw();
	};
	class ServerlessSectionException : public std::exception {
		virtual const char *what() const throw();
	};
};
