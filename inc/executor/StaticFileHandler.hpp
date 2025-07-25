#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"

class StaticFileHandler
{
	public:
		static void handleStaticFile(Conversation& conv);
};
