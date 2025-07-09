#include "ConfigStore.hpp"

ConfigStore::ConfigStore(const std::vector<serverConfig>& preInitConfigs) :_preInitConfigs(preInitConfigs) {}

ConfigStore::~ConfigStore(void) {}

void	ConfigStore::bindSocketToConfig(const int serverSocket, const serverConfig& config) {

	_configs[serverSocket] = config;
}

const std::vector<serverConfig>&	ConfigStore::getPreInitConfigs(void) const {

	return (_preInitConfigs);
}

serverConfig	ConfigStore::getConfig(int serverSocket) const {

	std::map<int, serverConfig>::const_iterator it = _configs.find(serverSocket);
	if (it == _configs.end()) {
		throw std::runtime_error("No configs found for the current socket");
	}
	return (it->second);
}

const std::map<int, serverConfig>& ConfigStore::getAllConfigs(void) const {

	return (_configs);
}
