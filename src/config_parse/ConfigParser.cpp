#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& configPath, const ConfigFileReader& configReader, const HeaderSectionParser& headerParser, const ServerSectionParser& serverParser,
	const LocationSectionParser& locationParser)
	: _configPath(configPath), _configReader(configReader), _headerParser(headerParser), _serverParser(serverParser), _locationParser(locationParser) {}

ConfigParser::~ConfigParser(void) {}

std::vector<serverConfig> ConfigParser::getConfigs(void) const {
	return (_configs);
}

void	ConfigParser::extractConfigs(const std::vector<std::string> formattedContent) {

	parserContext context;

	for (std::vector<std::string>::const_iterator it = formattedContent.begin(); it != formattedContent.end(); ++it)  {
		switch (context.state) {
			case START || HEADER_SECTION:
				_headerParser.handleCurrentHeader(*it, &context);
			case SERVER_SECTION :
				_serverParser.extractCurrentDirective(*it, &context);
			case LOCATION_SECTION :
				_locationParser.extractCurrentDirective(*it, &context);
		}
	}
}

void	ConfigParser::parse(void) {

	_rawConfigContent = _configReader.loadConfigContent(_configPath);
	std::string removedComments = _commentsRemover.remove(_rawConfigContent);
	std::vector<std::string> formatted = _formatter.format(removedComments);

	extractConfigs(formatted);
}
