#pragma once
#include <map>
#include <string>

typedef std::map<std::string, std::string> mapStr;
typedef std::pair<std::string, std::string> pairStr;

const size_t npos = std::string::npos;
const std::string loalpha = "abcdefghijklmnopqrstuvwxyz";
const std::string upalpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string alpha = loalpha + upalpha;
const std::string tchar = alpha + "!#$%&'*+-.^_|~";

void toLower(std::string& s);
void parseThrow(std::string what);
std::string extractToken(std::string& s);
void deleteLeadOWS(std::string& s);
void deleteTrailOWS(std::string& s);
void deleteQuotedString(std::string& s);
void deleteChunkExt(std::string& s);
