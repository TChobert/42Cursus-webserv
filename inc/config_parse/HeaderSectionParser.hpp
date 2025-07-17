#pragma once

#include <string>
#include <iostream>
#include <exception>
#include <sstream>
#include <cstring>

#include "ConfigParser.hpp"

class HeaderSectionParser {

	private:

	static const char HEADER_OPEN = '[';
	static const char HEADER_CLOSE = ']';
	static const char* const LOCATION_KEYWORD;
	static const char* const SERVER_KEYWORD;
	enum headerType {SERVER, LOCATION};

	void ensureHeaderIsEnclosed(const std::string& header);
	void getLocationName(const std::string& header, parserContext* context);
	std::string trimHeader(const std::string& header);
	headerType getHeaderType(const std::string& header);

	public:

	HeaderSectionParser(void);
	~HeaderSectionParser(void);

	void handleCurrentHeader(const std::string& header, parserContext *context);

	class InvalidHeaderException : public std::exception {
		virtual const char *what() const throw();
	};
	class ServerlessSectionException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidLocationName : public std::exception {
		virtual const char *what() const throw();
	};
};
