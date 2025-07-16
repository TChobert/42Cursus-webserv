#pragma once

#include <cstdlib>
#include <string>
#include <iostream>
#include <exception>
#include <vector>

#include "CommentsRemover.hpp"
#include "serverConfig.hpp"
#include "ConfigFileReader.hpp"
#include "Formatter.hpp"
#include "ServerConfigParser.hpp"
#include "LocationConfigParser.hpp"

class ServerConfigParser;
class LocationConfigParser;

enum currentState {
	START,
	SECTION_HEADER,
	SERVER,
	LOCATION,
	END,
};

struct parserState {
	currentState state;
	bool isInServerScope;
};

class ConfigParser {

	private:

	std::string _configPath;
	std::string _rawConfigContent;
	const ConfigFileReader _configReader;
	CommentsRemover _commentsRemover;
	Formatter _formatter;
	ServerConfigParser _serverParser;
	LocationConfigParser _locationParser;
	std::vector<serverConfig> _configs;

	void getSectionHeaderType(const std::string& header, parserState *state);
	void extractConfigs(const std::vector<std::string> formattedContent);

	public:

	ConfigParser(const std::string& configPath, const ConfigFileReader& configReader, const ServerConfigParser& serverParser,
		const LocationConfigParser& locationParser);
	~ConfigParser(void);

	std::vector<serverConfig>	getConfigs(void) const;
	void parse(void);

};
