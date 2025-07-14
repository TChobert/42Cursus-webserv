#pragma once

#include "webserv.hpp"

class IModule {

	public:

	virtual void	execute(Conversation& conv) = 0;
	virtual			~IModule(void);
};
