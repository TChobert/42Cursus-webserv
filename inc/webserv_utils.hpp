#pragma once
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <ctime>
#include "webserv_enum.hpp"

class Conversation;

const std::string base10 = "0123456789";
const std::string base16 = base10 + "abcdefABCDEF";

void		toLower(std::string& s, size_t start = 0, size_t len = SIZE_MAX);
//Throws "invalid_argument" (if s doesnt start with a proper size_t) or "overflow_error" !
size_t		extractSize(std::string& s, int base=10);
//Doesnt change s
size_t		peekSize(std::string& s, int base=10);
void		earlyResponse(Conversation& conv, statusCode status, bool close=true);
std::string	getCurrentHttpDate();
std::string	intToString(int n);
std::string	getFileExtension(const std::string& path);
std::string	getMimeType(const std::string& path);
void		freeEnv(char **envp);
std::string trim(const std::string& str);
bool		hasCgiProcessExitedCleanly(pid_t cgiPid);
