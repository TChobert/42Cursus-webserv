#include "../../inc/parse/parse.hpp"

using namespace std;
using namespace ParserRoutine;

string parseBody(string& s, bool isChunked) {
	if (isChunked)
		return parseBodyChunked(s);
	return parseBodyUnchunked(s, conv.request.header["Content-Size"]);
	

std::string parseBodyUnchunked(stdStr& s, int len);
std::string parseBodyChunked(stdStr& s);
