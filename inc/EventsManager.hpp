#pragma once

#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
#include <stdexcept>
#include <map>
#include <set>

#include "ConfigStore.hpp"
#include "ServerInitializer.hpp"
#include "Dispatcher.hpp"
#include "Conversation.hpp"

#define MAX_EVENTS 64 // arbitraire, a changer ou non

class EventsManager {

	private:

	ConfigStore&				_configs; // stocker les configurations des differents serveurs -> initialisee par le parsing du fichier de config
	ServerInitializer&			_initializer;
	Dispatcher&					_dispatcher;
	std::set<int>				_listenSockets; // stocker les sockets du ou des serveurs actifs et en mode ecoute
	std::map<int,Conversation>	_clients; // stocker le contexte de chaque client -> un fd associe a un contexte
	int							_epollFd; // maintenir et interagir avec l'instance epoll
	struct epoll_event			_events[MAX_EVENTS]; // buffer a passer a epoll_wait pour qu'il puisse ecrire les evenements

	public:

	EventsManager(ConfigStore& configs, ServerInitializer& initializer, Dispatcher& dispatcher);
	~EventsManager(void);

	void	run(void); // boucle principale, epoll, arrivee clients, appels dispatcher, etc
	void	listenEvents(void);
	void	acceptNewClient(int fd); // accepter la demande de connexion d'un nouveau client, initialiser son contexte
	void	handleClientEvent(int fd);
	void	deleteClient(int fd); // supprimer un client si conversation close, erreur ou timeout -> a voir si ici ?
	void	deleteAllClients(void);
	void	deleteServers(void);
	void	deleteAllNetwork(void);
};
