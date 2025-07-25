#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"

class CGIHandler
{
	private:


	public:
		static bool isCGI(const Conversation& conv);
		static void handleCGI(const Conversation& conv);
};
