#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <map>

class ServerSectionParser;
class ConfigParser;
struct parserContext;

class ServerPropertiesProcessor {

	private:

	enum keyType{PORT, HOST, NAME, ROOT, ERROR_PAGE, UNKNOWN};
	static const int validErrorCodes[];

	keyType getKeyType(const std::string& directiveKey);
	int getErrorCodeValue(const std::string& errorCode);
	bool isErrorPathValid(const std::string& pagePath);
	void addErrorPageToErrorMap(parserContext *context, int codeValue, std::string pagePath);
	bool isValueInMap(const std::map<int, std::string>& map, const std::string& value);

	public:

	typedef void (ServerPropertiesProcessor::*ProcessPtr)(const std::string&, parserContext*);
	ServerPropertiesProcessor(void);
	~ServerPropertiesProcessor(void);

	void processPortProperty(const std::string& property, parserContext *context);
	void processHostProperty(const std::string& property, parserContext *context);
	void processNameProperty(const std::string& property, parserContext *context);
	void processRootProperty(const std::string& property, parserContext *context);
	void processErrorPageProperty(const std::string& property, parserContext *context);
	//void ensureServerConfigIsFull(parserContext *context);

	ProcessPtr getPropertyProcess(const std::string& propertyKey);

	class InvalidPropertyException : public std::exception {
	public:
		virtual const char *what() const throw();
	};
	class InvalidPortPropertyException : public std::exception {
	public:
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
	class InvalidErrorPageException : public std::exception {
		virtual const char *what() const throw();
	};
	class DoublePropertyException : public std::exception {
		virtual const char *what() const throw();
	};
	class UnknownErrorCodeException : public std::exception {
		virtual const char *what() const throw();
	};
	// class MissingPropertyException : public std::exception {
	// public:
	// 	virtual const char *what() const throw();
	// };
};
