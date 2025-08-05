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

class ServerSectionParser;
class ConfigParser;
struct parserContext;

class ServerPropertiesProcessor {

	private:

	enum keyType {PORT, HOST, NAME, ROOT, ERROR_PAGE, UNKNOWN};
	static const char* forbiddenPaths[];
	static const int validErrorCodes[];

	keyType getKeyType(const std::string& directiveKey);
	void ensureRootIsAllowed(const std::string& root);
	void ensureRootIsValid(const std::string& root);
	int getErrorCodeValue(const std::string& errorCode);
	void ensureErrorPathIsValid(const std::string& pagePath);
	void addErrorPageToErrorMap(parserContext *context, int codeValue, std::string pagePath);
	bool isValueInMap(const std::map<int, std::string>& map, const std::string& value);

	public:

	typedef void (ServerPropertiesProcessor::*ServerProcessPtr)(const std::string&, parserContext*);
	ServerPropertiesProcessor(void);
	~ServerPropertiesProcessor(void);

	void processPortProperty(const std::string& property, parserContext *context);
	void processHostProperty(const std::string& property, parserContext *context);
	void processNameProperty(const std::string& property, parserContext *context);
	void processRootProperty(const std::string& property, parserContext *context);
	void processErrorPageProperty(const std::string& property, parserContext *context);

	ServerProcessPtr getPropertyProcess(const std::string& propertyKey);

	class InvalidPropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidPortPropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidHostPropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidNamePropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidServerRootException : public std::exception {
		virtual const char *what() const throw();
	};
	class ForbiddenServerRootException : public std::exception {
		virtual const char *what() const throw();
	};
	class InvalidErrorPageException : public std::exception {
		virtual const char *what() const throw();
	};
	class DoublePropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class UnknownErrorCodeException : public std::exception {
		virtual const char *what() const throw();
	};
	class DoubleErrorPathException : public std::exception {
		virtual const char *what() const throw();
	};
	class UnexistantErrorPageException : public std::exception {
		virtual const char *what() const throw();
	};
};
