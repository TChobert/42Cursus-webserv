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
	static const char * cgiExtensions[];

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
	void fetchUploadAuthorisation(const std::string& property, parserContext *context);
	void processUploadDirProperty(const std::string& property, parserContext *context);
	void fetchAutoIndex(const std::string& property, parserContext *context);
	void processIndexProperty(const std::string& property, parserContext *context);
	void processCgiProperty(const std::string& property, parserContext *context);
	void getCgiExtensionAndHandler(std::map<std::string, std::string>& cgiRules, const std::string& cgiRule);
	bool isValidCgiExtension(const std::string& extension) const;
	bool isValidCgiHandler(const std::string& extension) const;

	bool isValidMethod(const std::string& method) const;

	LocationProcessPtr getLocationPropertyProcess(const std::string& key);

	class InvalidLocationPropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class EmptyLocationPropertyException : public std::exception {
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
	class InvalidAutoIndexException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidCgiException : public std::exception {
		virtual const char *what() const throw();
	};
};
