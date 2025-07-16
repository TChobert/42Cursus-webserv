#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& configPath, const ConfigFileReader& configReader)
	: _configPath(configPath), _configReader(configReader) {}

ConfigParser::~ConfigParser(void) {}

std::vector<serverConfig>	ConfigParser::parse(void) {

	_rawConfigContent = _configReader.loadConfigContent(_configPath);
	std::cout << _rawConfigContent;
}
