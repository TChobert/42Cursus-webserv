#pragma once
#include <string>
#include <map>
#include <vector>
#include <exception>
#include "parse_utils.hpp"

typedef std::string str;
typedef std::map<str, str> mapStr;
typedef std::pair<str, str> pairStr;

struct startLine {
	std::string method;
	std::string requestURI;
	std::pair<int,int> version;
};

startLine parseStartLine(std::string& s);
std::string extractMethod(std::string& s);
std::string extractRequestURI(std::string& s);
std::pair<int, int> extractHTTPVersion(std::string& s);

struct requestHeader {
	mapStr map;
};

/*
enum method {
	GET,
	POST,
	DELETE,
	METH_NB
};

enum parserState {
	NEED_READ,
	NEED_VALID,
	PARSE_ERROR,
	TOO_LARGE,
	DONE
};

class Parser {
private:
	std::string		buf;
	std::vector<std::string>	head;
	std::string		body;
public:
	parserState		state;
	void			getHeader();
	void			getBody();

};
*/
