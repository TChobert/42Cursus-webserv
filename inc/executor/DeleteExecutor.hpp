#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "ResourceChecker.hpp"
#include "CGIHandler.hpp"
#include "StaticFileHandler.hpp"

class DeleteExecutor
{
	public:
		static void handleDelete(Conversation&);
};
