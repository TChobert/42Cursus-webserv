#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"

class CGIHandler
{
	private:
		static char**	prepareEnv(Conversation& conv);
		static void		handleGetCGI(Conversation& conv);
		static void		handlePostCGI(Conversation& conv);

	public:
		static bool		isCGI(const Conversation& conv);
		static void		handleCGI(Conversation& conv);
		static void		parseCgiOutput(Conversation& conv);
};
