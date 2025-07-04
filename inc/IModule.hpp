#pragma once

class Conversation;

class IModule {

	public:

	virtual void	execute(Conversation& conversation) = 0;
	virtual			~IModule(void);
};
