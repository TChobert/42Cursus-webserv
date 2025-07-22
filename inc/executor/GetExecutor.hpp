#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "ResourceChecker.hpp"
#include "CGIHandler.hpp"
#include "StaticFileHandler.hpp"

class GetExecutor
{
	public:
		static void handleGet(Conversation&);
		static void handleFile(Conversation& conv);
		static void handleDirectory(Conversation& conv);
		static void handleAutoindex(Conversation& conv);
		static void resumeStatic(Conversation&);
		static void resumeReadCGI(Conversation&);
		static void resumeWriteCGI(Conversation&);
};

