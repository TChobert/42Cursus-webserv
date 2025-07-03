#pragma once

#include <cstdlib>
#include <vector>

struct serverConfig;

class ConfigStore {

	private:

	std::vector<serverConfig>	_configs;

	public:

	ConfigStore(const std::vector<serverConfig>& configs);
	ConfigStore(const ConfigStore& other);
	ConfigStore&	operator=(const ConfigStore& other);
	~ConfigStore(void);

	//Fonction de matching a implementer
	const serverConfig&					getServerConfig(int index);
	const std::vector<serverConfig>&	getAllConfigs(void);
	size_t								size(void);
};

// CONFIG STORE //

// Classe utilisee pour stocker la ou les configurations identifiees et extraites lors du parsing du *.conf

// Utile pour deux raisons avant tout :

//	- Initialiser chaque serveur (-> EventsManager)
//	- Lorsque que le parsing de la requete d'un client a ete effectuee, recuperer la config via le hostname et verifier que tout est ok
