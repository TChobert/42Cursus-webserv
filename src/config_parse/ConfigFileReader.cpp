#include "ConfigFileReader.hpp"

ConfigFileReader::ConfigFileReader(void) {}

ConfigFileReader::~ConfigFileReader(void) {}

std::string	ConfigFileReader::loadConfigContent(const std::string& configPath) const {

	std::ifstream fileStream(configPath.c_str());

	if (!fileStream.is_open()) {
		throw OpenFailureException();
	}

	std::stringstream rawConfigContent;
	rawConfigContent << fileStream.rdbuf();

	return rawConfigContent.str();
}

// EXCEPTIONS //

const char	*ConfigFileReader::OpenFailureException::what() const throw() {

	return ("Error: webserv: cannot open configuration file");
}
