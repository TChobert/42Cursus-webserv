#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <map>
#include <vector>

#include "webserv_utils.hpp"

class LocationSectionParser;
class ConfigParser;
struct parserContext;

class LocationPropertiesProcessor {

	protected:

	static const char SPACE = ' ';
	enum keyType {
		ROOT,
		ALLOWED_METHODS,
		UPLOAD_AUTH,
		UPLOAD_DIR,
		AUTOINDEX,
		INDEX,
		CGI,
		RETURN,
		BODY_SIZE,
		ERROR_PAGE,
		UNKNOWN
	};

	public:

	typedef void (LocationPropertiesProcessor::*LocationProcessPtr)(const std::string&, parserContext*);

	LocationPropertiesProcessor(void);
	~LocationPropertiesProcessor(void);

	keyType getKeyType(const std::string& directiveKey); // PUBLIC JUST FOR TESTING !!
	void processMethodsProperty(const std::string& property, parserContext *context);
	void fetchUploadAuthorisation(const std::string& property, parserContext*context);

	bool isValidMethod(const std::string& method) const;

	LocationProcessPtr getLocationPropertyProcess(const std::string& key);

	class InvalidLocationPropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidUploadAuthException : public std::exception {
		virtual const char *what() const throw();
	};
	class DoubleLocationPropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidMethodException : public std::exception {
		virtual const char *what() const throw();
	};
};
