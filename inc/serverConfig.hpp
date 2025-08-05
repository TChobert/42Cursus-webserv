#pragma once

#include "webserv_enum.hpp"
#include <string>
#include <map>
#include <vector>

typedef std::string locationName;
typedef std::string cgiExtension;
typedef std::string cgiHandler;

// Information for each location
struct locationConfig {

	locationName				name;
	bool						hasRoot;
	std::string					root;
	std::vector<std::string>	allowedMethods;
	std::string					index;
	bool						autoIndex;
	size_t						clientMaxBodySize;
	bool						hasRedir;
	statusCode					redirCode;
	std::string					redirURL;
	bool						uploadEnabled;
	std::string					uploadStore;
	std::map<cgiExtension, cgiHandler> cgiHandlers;
};

struct serverInfo {

	uint16_t		port;
	std::string		host;
	std::string		serverName;
	std::string		root;
	bool			hasRoot;
};

struct serverConfig {

	serverInfo		identity;
	std::map<int, std::string>	errorPagesCodes;
	std::map<locationName, locationConfig> locations;
	// CONSTRUCTOR ??
};

// std::map<int, int> m;
//  m[2] = 4;
//  m[3] = 6;
//  if (m[2] == 4)
// 	do();
// if ( m.count(5) && m[5] == 63)
// 	do();
