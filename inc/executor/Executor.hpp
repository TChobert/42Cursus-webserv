#pragma once

#include "IModule.hpp"
#include "webserv_enum.hpp"
#include "webserv.hpp"
#include "GetExecutor.hpp"
#include "PostExecutor.hpp"
#include "DeleteExecutor.hpp"

class Executor : public IModule
{
	public:
		void	execute(Conversation& conv);
		void	resume(Conversation& conv);
};
