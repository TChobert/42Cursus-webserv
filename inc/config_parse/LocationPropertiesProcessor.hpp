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

class LocationSectionParser;
class ConfigParser;
struct parserContext;

class LocationPropertiesProcessor {

	private:

	enum keyType {
		ROOT,
		ALLOWED,
		UPLOAD_ENABLED,
		UPLOAD_STORE,
		AUTOINDEX,
		INDEX,
		CGI,
		RETURN,
		BODY_SIZE,
		ERROR_PAGE,
		UNKNOWN
	};

	keyType getKeyType(const std::string& directiveKey);

	public:

	typedef void (LocationPropertiesProcessor::*LocationProcessPtr)(const std::string&, parserContext*);
	LocationPropertiesProcessor(void);
	~LocationPropertiesProcessor(void);

	LocationProcessPtr getLocationPropertyProcess(const std::string& propertyKey);
};
