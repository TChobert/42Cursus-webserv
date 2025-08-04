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

#define MAX_BODY_VALUE_ALLOWED 4294967295L
#define DEFAULT_BODY_SIZE (1 * 1024 * 1024)

class LocationSectionParser;
class ConfigParser;
struct parserContext;

class LocationPropertiesProcessor {

	protected:

	static const char SPACE = ' ';
	static const char * cgiExtensions[];
	static const char* forbiddenPaths[];
	static const std::string httpPrefix;
	static const std::string httpsPrefix;

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
		UNKNOWN
	};

	public:

	typedef void (LocationPropertiesProcessor::*LocationProcessPtr)(const std::string&, parserContext*);

	LocationPropertiesProcessor(void);
	~LocationPropertiesProcessor(void);

	keyType getKeyType(const std::string& directiveKey); // PUBLIC JUST FOR TESTING !!
	void processLocationRootProperty(const std::string& property, parserContext *context);
	void processMethodsProperty(const std::string& property, parserContext *context);
	void fetchUploadAuthorisation(const std::string& property, parserContext *context);
	void processUploadDirProperty(const std::string& property, parserContext *context);
	void fetchAutoIndex(const std::string& property, parserContext *context);
	void processIndexProperty(const std::string& property, parserContext *context);
	void processCgiProperty(const std::string& property, parserContext *context);
	void fetchLocationReturnInfo(const std::string& property, parserContext *context);
	void fetchMaxBodySize(const std::string& property, parserContext *context);

	void ensureRootIsAllowed(const std::string& root) const;
	void getCgiExtensionAndHandler(std::map<std::string, std::string>& cgiRules, const std::string& cgiRule);
	bool isValidCgiExtension(const std::string& extension) const;
	bool isValidCgiHandler(const std::string& extension) const;
	bool isValidMethod(const std::string& method) const;
	statusCode getReturnCode(const std::string& codeStr) const;
	bool isValidUrl(const std::string& url) const;
	size_t getMaxBodyValueByUnit(long value, const std::string& unit);
	void ensureUploadDirIsValid(const std::string& dir) const;
	void ensureUploadDirIsAllowed(const std::string& dir) const;

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
	class ForbiddenLocationRootException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidUploadDirException : public std::exception {
		virtual const char *what() const throw();
	};
	class ForbiddenUploadDirException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidLocationReturnException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidBodySizeException : public std::exception {
		virtual const char *what() const throw();
	};
};
