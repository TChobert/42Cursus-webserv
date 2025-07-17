#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"

class StaticFileHandler
{
	private:


	public:
		static void handleStaticFile(const Conversation& conv);
};
