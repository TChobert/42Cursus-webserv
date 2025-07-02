#include <string>
#include <map>
#include <vector>

enum method {
	GET,
	POST,
	DELETE,
	METH_NB
};

enum parserState {
	NEED_READ,
	NEED_VALID,
	ILLEGAL,
	UNKNOWN_METHOD,
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
