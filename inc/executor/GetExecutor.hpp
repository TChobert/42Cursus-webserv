#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "ResourceChecker.hpp"
#include "CGIHandler.hpp"
#include "StaticFileHandler.hpp"

class GetExecutor
{
	private:

	public:
		static void handleGet(Conversation& conv);
		static void handleFile(Conversation& conv);
		static void handleDirectory(Conversation& conv);
		static void handleAutoindex(Conversation& conv);
		static void resumeStatic(Conversation& conv);
		static void resumeReadCGI(Conversation& conv);
		static void resumeWriteCGI(Conversation& conv);
};

