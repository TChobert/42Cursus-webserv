#pragma once
#include <map>
#include <string>
#include <iostream>

typedef std::map<std::string, std::string> mapStr;
typedef std::pair<std::string, std::string> pairStr;

const size_t npos = std::string::npos;
const std::string loalpha = "abcdefghijklmnopqrstuvwxyz";
const std::string upalpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string alpha = loalpha + upalpha;
const std::string tchar = alpha + "!#$%&'*+-.^_|~";

void toLower(std::string& s);
void parseThrow(std::string what);
//Throws on negative and overflow !
int extractInt(std::string& s);
std::string extractToken(std::string& s);
void deleteLeadOWS(std::string& s);
void deleteTrailOWS(std::string& s);
