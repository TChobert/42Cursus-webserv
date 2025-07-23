#pragma once

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include "webserv.hpp"
#include "webserv_enum.hpp"

class Conversation;

class ResourceChecker
{
	public:
		static bool exists(const std::string& path);
		static bool isFile(const std::string& path);
		static bool isDir(const std::string& path);
		static bool isReadable(const std::string& path);
		static bool isExecutable(const std::string& path);
		static statusCode checkAccess(const std::string& path);
		static bool canDelete(const std::string& path);
};
