#pragma once

#include "webserv_enum.hpp"
#include <string>
#include <map>
#include <vector>

typedef std::string locationName;

// Information for each location
struct locationConfig {

	locationName				name;
	bool						hasRoot;
	std::string					root;
	std::vector<std::string>	allowedMethods;
	std::string					index;
	bool						autoIndex;
	bool						hasRedir;
	statusCode					redirCode;
	std::string					redirURL;
};

// Error pages paths. Optionnal for some of them. We can add more later or remove some of them if necessary
struct errorPages {

	std::string	error_400;
	std::string	error_403;
	std::string	error_404;
	std::string	error_405;
	std::string	error_408;
	std::string	error_413;
	std::string	error_500;
	std::string	error_501;
	std::string	error_504;
};

struct serverInfo {

	unsigned int	port;
	std::string		host;
	std::string		server_name;
};

struct serverConfig {

	serverInfo		identity;
	errorPages		errorPagesPaths;
	std::map<int, std::string>	errorPagesCodes;
	std::map<locationName, locationConfig> locations;
};

// std::map<int, int> m;
//  m[2] = 4;
//  m[3] = 6;
//  if (m[2] == 4)
// 	do();
// if ( m.count(5) && m[5] == 63)
// 	do();
