#pragma once

#include <string>
#include <map>
#include <vector>

typedef std::string locationName;

// Information for each location
struct locationConfig {

	locationName				name;
	std::vector<std::string>	allowedMethods;
	std::string					index;
	bool						autoIndex;
	size_t						clientMaxBodySize;
	bool						hasReturn;
	int							returnCode;
	std::string					returnURL;
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
	std::string		root; // move into locationConfig ?
};

struct serverConfig {

	serverInfo		identity;
	errorPages		errorPagesPaths;
	std::map<locationName, locationConfig> locations;
};
