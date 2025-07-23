#pragma once

#include "IModule.hpp"
#include "webserv_enum.hpp"
#include "webserv.hpp"
#include "GetExecutor.hpp"
#include "PostExecutor.hpp"
#include "DeleteExecutor.hpp"
#include "execState.hpp"

class Executor : public IModule
{
	public:
		void		execute(Conversation& conv);
		void		executeStart(Conversation& conv);
		static void	updateResponseData(Conversation& conv);
		static void	setResponse(Conversation& conv, statusCode code);
};
