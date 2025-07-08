#pragma once

#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <map>

#include "serverConfig.hpp"

class ConfigStore {

	private:

	const std::vector<serverConfig>	_preInitConfigs;
	std::map<int, serverConfig>	_configs;

	public:

	ConfigStore(const std::vector<serverConfig>& preInitConfigs);
	~ConfigStore(void);

	void									bindSocketToConfig(const int serverSocket, const serverConfig& config);

	const std::vector<serverConfig>&		getPreInitConfigs(void) const;
	serverConfig							getConfig(int serverSocket) const;
	const std::map<int, serverConfig>&		getAllConfigs(void) const;
	size_t									size(void);
};
