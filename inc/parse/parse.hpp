#pragma once
#include <string>
#include "parse_utils.hpp"
#include "Parser.hpp"

const size_t startLineMax = 1e4;
const size_t methodMax = 8;
const size_t uriMax = 8000;
const size_t headerMax = 1e4;

namespace ParserRoutine {
	std::string extractMethod(std::string& s);
	std::string extractRequestUri(std::string& s);
	std::pair<int, int> extractHttpVersion(std::string& s);

	bool isValidFieldValue(std::string& s);
	mapStr parseAllField(std::string& s);
	pairStr extractOneField(std::string& s);

	std::string parseBodyChunked(std::string& s);
};
