#pragma once

class Conversation;
class IModule {

	public:

	virtual void	execute(Conversation& conv) = 0;
	virtual			~IModule(void);
};
