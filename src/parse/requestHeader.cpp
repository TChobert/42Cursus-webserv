#include "../../inc/parse/parse.hpp"

requestHeader parseRequestHeader(std::string& s) {
	requestHeader res;
	while (s != "") {
		pairStr h = extractOneHeader(s);
		if (res.map.count(h.first)) {
			mapStr::iterator it = res.map.find(h.first);
			it->second += ',';
			it->second += h.second;
		} else
			res.map[h.first] = h.second;
	}
	return res;
}

pairStr extractOneHeader(std::string& s) {

