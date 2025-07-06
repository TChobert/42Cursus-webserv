#include "../../inc/parse/parse.hpp"

using namespace std;
using namespace ParserRoutine;

std::string parseBodyUnchunked(stdStr& s, int len);
std::string parseBodyChunked(stdStr& s);
