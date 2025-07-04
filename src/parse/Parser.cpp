#include "../../inc/parse/Parser.hpp"

using namespace std;
using namespace ParserRoutine;

void Parser::parseStartLine(Conversation& conv) {
	conv.req.method = extractMethod(buf[conv.fd]);
	conv.req.uri = extractRequestURI(buf[conv.fd]);
	conv.req.method = extractMethod(buf[conv.fd]);
}
/*
void	Parser::parseStartLine() {
	size_t pos = head[0].find(" ");
	if (pos == std::string::npos) {
		state = ILLEGAL;
		return;
	}
	std::string m = head[0].substr(0, pos);
	if (m == "GET")
		request.method = GET;
	else if (m == "POST")
		request.method = POST;
	else if (m == "DELETE")
		request.method = DELETE;
	else 

	
}
void	Parser::getHeader() {
	while (true) {
		size_t pos = buf.find("\r\n");
		if (pos == std::string::npos) {
			state = NEED_READ;
			return;
		}
		if (pos == 0) {
			buf.erase(0, 2);
			if (head.size())
				break;
			else
				continue;
		}
		head.emplace_back(buf.substr(0, pos));
		buf.erase(0, pos + 2);
	}
	parseStartLine();
	parseHeader();
}
*/
