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
#include "ServerSectionParser.hpp"
#include "LocationSectionParser.hpp"
#include "HeaderSectionParser.hpp"

enum currentState {
	START,
	HEADER_SECTION,
	SERVER_SECTION,
	LOCATION_SECTION,
	END,
};

struct parserState {
	currentState state;
	bool isInServerScope;
	bool isConfigComplete;
	serverConfig currentConfig;
};

class ConfigParser {

	private:

	std::string _configPath;
	std::string _rawConfigContent;
	const ConfigFileReader _configReader;
	CommentsRemover _commentsRemover;
	Formatter _formatter;
	HeaderSectionParser _headerParser;
	ServerSectionParser _serverParser;
	LocationSectionParser _locationParser;
	std::vector<serverConfig> _configs;

	void extractConfigs(const std::vector<std::string> formattedContent);

	public:

	ConfigParser(const std::string& configPath, const ConfigFileReader& configReader, const HeaderSectionParser& headerParser, const ServerSectionParser& serverParser,
		const LocationSectionParser& locationParser);
	~ConfigParser(void);

	std::vector<serverConfig>	getConfigs(void) const;
	void parse(void);
};
