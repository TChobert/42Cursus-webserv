#pragma once

#include "IModule.hpp"
#include "webserv_enum.hpp"
#include "webserv.hpp"
#include "GetExecutor.hpp"
#include "PostExecutor.hpp"
#include "DeleteExecutor.hpp"

enum execState
{
	EXEC_START,
	READ_EXEC_GET_STATIC,
	WRITE_EXEC_GET_AUTOINDEX,
	READ_EXEC_GET_CGI,
	WRITE_EXEC_GET_CGI,
	READ_EXEC_POST_BODY,
	WRITE_EXEC_POST_UPLOAD,
	READ_EXEC_POST_CGI,
	WRITE_EXEC_POST_RESPONSE,
	WRITE_EXEC_DELETE,
};

class Executor : public IModule
{
	public:
		void	execute(Conversation& conv);
		void	executeStart(Conversation& conv);
};
