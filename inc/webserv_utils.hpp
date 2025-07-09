#pragma once
#include <string>
#include "webserv_enum.hpp"

class Conversation;

const std::string base10 = "0123456789";
const std::string base16 = base10 + "abcdefABCDEF";

void toLower(std::string& s);
//Throws "invalid_argument" (if s doesnt start with a proper size_t) or "overflow_error" !
size_t extractSize(std::string& s, int base=10);
//Doesnt change s
size_t peekSize(std::string& s, int base=10);
void earlyResponse(Conversation& conv, statusCode status, bool close=true);
