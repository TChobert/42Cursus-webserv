#include "ServerInitializer.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <unistd.h>
#include <cassert>

int main() {

	try {
		// Création d'une configuration minimale
		serverConfig config;
		config.identity.host = "127.0.0.1";
		config.identity.port = 8080;

		// Création d'un vecteur avec la config
		std::vector<serverConfig> configs;
		configs.push_back(config);

		// Instanciation du store de config
		ConfigStore configStore(configs);

		// Création d'un epoll fd (compatible C++98)
		int epollFd = epoll_create(10); // size >= 1
		if (epollFd == -1) {
			std::cerr << "❌ epoll_create failed\n";
			return 1;
		}

		// Initialisation du serveur
		ServerInitializer initializer(configStore, epollFd);

		std::set<int> serverSockets = initializer.initServers();

		if (serverSockets.empty()) {
			std::cerr << "❌ Aucun socket serveur initialisé\n";
			return 1;
		}

		std::cout << "✅ " << serverSockets.size() << " socket(s) initialisé(s)\n";

		// Nettoyage
		std::set<int>::iterator it;
		for (it = serverSockets.begin(); it != serverSockets.end(); ++it) {
			close(*it);
		}
		close(epollFd);

		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "❌ Exception capturée : " << e.what() << std::endl;
		return 1;
	}
}

// TO COMPILE :

// c++ -std=c++98 -Wall -Wextra -Werror \
//     -Iinc \
//     -Iinc/events_manager \
//     -Iinc/configs_store \
//     test/server_initializer/main.cpp \
//     src/events_manager/ServerInitializer.cpp \
//     src/config_store/ConfigStore.cpp \
//     -o test_server_init
