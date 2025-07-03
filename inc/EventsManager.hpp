#pragma once

#include <cstdlib>
#include <sys/epoll.h>
#include <map>
#include <set>

#define MAX_EVENTS 64 // arbitraire, a changer ou non

class ConfigStore;
class ClientContext;
class Dispatcher;

class EventsManager {

	private:

	ConfigStore&				_configs; // stocker les configurations des differents serveurs -> initialisee par le parsing du fichier de config
	Dispatcher&					_dispatcher;
	std::set<int>				_listenSockets; // stocker les sockets du ou des serveurs actifs et en mode ecoute
	std::map<int,ClientContext>	_clients; // stocker le contexte de chaque client -> un fd associe a un contexte
	int							_epollFd; // maintenir et interagir avec l'instance epoll
	struct epoll_event			_events[MAX_EVENTS]; // buffer a passer a epoll_wait pour qu'il puisse ecrire les evenements

	public:

	EventsManager(ConfigStore& configs, Dispatcher& Dispatcher);
	EventsManager(const EventsManager& other);
	EventsManager&	operator=(const EventsManager& other);
	~EventsManager(void);

	void	initServers(void); // Pour initialiser le ou les serveurs
	void	run(void); // boucle principale, epoll, arrivee clients, appels dispatcher, etc
	void	acceptNewClient(int fd); // accepter la demande de connexion d'un nouveau client, initialiser son contexte
	void	deleteClient(int fd); // supprimer un client si conversation close, erreur ou timeout -> a voir si ici ?
};
