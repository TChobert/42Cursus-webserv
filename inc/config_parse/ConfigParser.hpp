#pragma once

#include <cstdlib>
#include <string>
#include <iostream>
#include <exception>
#include <vector>

#include "CommentsRemover.hpp"
#include "serverConfig.hpp"
#include "ConfigFileReader.hpp"

class ConfigParser {

	private:

	std::string _configPath;
	std::string _rawConfigContent;
	ConfigFileReader _configReader;
	CommentsRemover _commentsRemover;

	public:

	ConfigParser(ConfigFileReader& configReader);
	~ConfigParser(void);

	std::vector<serverConfig>	parse(std::string& configContent);
};
