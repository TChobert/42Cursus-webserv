#pragma once

#include <cstdlib>
#include <vector>
#include <map>

struct serverConfig;

class ConfigStore {

	private:

	std::vector<serverConfig> _configsBeforeInit;
	std::map<int, serverConfig>	_configs;

	public:

	ConfigStore(const std::vector<serverConfig>& configsBeforeInit);
	~ConfigStore(void);

	void									bindSocketToConfig(int fd, const serverConfig& config);
	const	serverConfig&					getConfig(int serverSocket);
	const std::map<int, serverConfig>&		getAllConfigs(void);
	size_t									size(void);
};
