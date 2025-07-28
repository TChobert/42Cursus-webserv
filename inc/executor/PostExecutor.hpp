#pragma once

#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"
#include "ResourceChecker.hpp"
#include "CGIHandler.hpp"
#include "StaticFileHandler.hpp"

class PostExecutor
{
	public:
		static void handlePost(Conversation&);
		static void resumePostWriteBodyToFile(Conversation&);
		static void resumePostWriteBodyToCGI(Conversation&);
		static void resumePostReadCGI(Conversation&);
};
