#pragma once

#include <cstdlib>
#include <vector>
#include <map>

#include "serverConfig.hpp"

class ConfigStore {

	private:

	std::vector<serverConfig>	_configsBeforeInit;
	std::map<int, serverConfig>	_configs;

	public:

	ConfigStore(const std::vector<serverConfig>& configsBeforeInit);
	~ConfigStore(void);

	void									bindSocketToConfig(int serverSocket, const serverConfig& config);
	const	serverConfig&					getConfig(int serverSocket);
	const std::map<int, serverConfig>&		getAllConfigs(void);
	size_t									size(void);
};
