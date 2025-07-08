#include "PostSender.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

bool PostSender::isKeepAlive(const Conversation& conv)
{
	std::map<std::string, std::string>::const_iterator it = conv.request.headers.find("Connection");

	if (it != conv.request.headers.end())
	{
		//ATTENTION: HTTP headers insensibles a la casse..
		//Keep-Alive / KEEP-ALIVE > Dans ce cas, faire une fonction toLower pour comparer directement
		return it->second == "keep-alive";
	}
	return (true); //par defaut, keep-alive ?
}

bool PostSender::isContinueStatus(const Conversation& conv)
{
	return conv.response.statusCode == 100;
}

/* ---------------- PUBLIC METHODS ------------------ */

void PostSender::execute(Conversation& conv)
{
	if (isContinueStatus(conv))
	{
		conv.state = PARSE_BODY;
		return ;
	}

	if (isKeepAlive(conv))
	{
		conv.state = PARSE_HEADER;
		conv.bytesSent = 0; // ou dispatcher qui reset ??
		conv.final_response.clear(); // ou dispatcher qui reset ? (tout serait centralise)
		return ;
	}

	conv.state = FINISH;
}
