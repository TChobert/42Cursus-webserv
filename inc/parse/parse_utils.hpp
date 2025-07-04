#pragma once
#include <map>
#include <string>

typedef std::map<std::string, std::string> mapStr;
typedef std::pair<std::string, std::string> pairStr;


const std::string separator = "()<>@,;:/[]?={} \t";
const std::string mark = "-_.!~*'()";
const std::string reserved = ";/?:@$=+$,";
const size_t npos = std::string::npos;

void parseThrow(std::string what);
//Throws on negative and overflow !
int extractInt(std::string& s);
bool isCTL(char c);
bool isUnreserved(char c);
void deleteLWS(std::string& s);
std::string extractUntilLWS(std::string& s);
bool isLegalToken(const std::string& s);
std::string extractToken(std::string& s);
