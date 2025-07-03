#pragma once
#include <string>

const std::string separator = "()<>@,;:/[]?={} \t";
const std::string mark = "-_.!~*'()";
const std::string reserved = ";/?:@$=+$,";
const size_t npos = std::string::npos;

bool isCTL(char c);
bool isUnreserved(char c);
bool isLegalToken(const std::string& s);
std::string extractToken(std::string& s);
