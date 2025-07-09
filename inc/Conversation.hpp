#pragma once

#include "serverConfig.hpp"

#include "webserv_enum.hpp"

class Conversation {

	public:

	int				_fd;
	serverConfig	_config;
	convState		_state;

	Conversation(void) : _fd(-1), _config(), _state(DRAIN_BUFFER) {};
};
