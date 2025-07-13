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
	const ConfigFileReader _configReader;
	CommentsRemover _commentsRemover;

	public:

	ConfigParser(const std::string& configPath, const ConfigFileReader& configReader);
	~ConfigParser(void);

	std::vector<serverConfig>	parse(void);
};
