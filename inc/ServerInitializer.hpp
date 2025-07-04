#pragma once

#include <set>
#include "ConfigStore.hpp"

class ServerInitializer {

	private:

	ConfigStore&	_configs;
	int				_epollFd;

	public:

	ServerInitializer(ConfigStore& configs, int epollFd);
	~ServerInitializer(void);

	std::set<int>	initServers(void);
};
