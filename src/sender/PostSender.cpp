#include "PostSender.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

bool PostSender::isKeepAlive(const Conversation& conv)
{
	std::map<std::string, std::string>::const_iterator it = conv.request.headers.find("Connection");

	if (it != conv.request.headers.end())
	{
		return toLower(it->second) == "keep-alive";
	}
	return (true); //par defaut, keep-alive
}

bool PostSender::isContinueStatus(const Conversation& conv)
{
	return conv.resp.statusCode == 100;
}

/* ---------------- PUBLIC METHODS ------------------ */

void PostSender::execute(Conversation& conv)
{
	if (isContinueStatus(conv))
	{
		conv.state = PARSE_BODY; //on doit envoyer headers + body en 2eme reponse (donc pas de clear de finalResponse)
		return ;
	}
	if (isKeepAlive(conv))
	{
		conv.state = PARSE;
		conv.bytesSent = 0;
		conv.finalResponse.clear();
		return ;
	}
	conv.state = FINISH;
}
