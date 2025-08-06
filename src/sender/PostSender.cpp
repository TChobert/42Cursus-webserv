#include "PostSender.hpp"
#include "ResponseBuilder.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

bool PostSender::isKeepAlive(const Conversation& conv)
{
	std::map<std::string, std::string>::const_iterator it = conv.req.header.find("Connection");

	if (it != conv.req.header.end())
	{
		std::string value = it->second;
		toLower(value);
		return value == "keep-alive";
	}
	return (true); //par defaut, keep-alive
}

bool PostSender::isContinueStatus(const Conversation& conv)
{
	return conv.resp.status == 100;
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
		ResponseBuilder::resetResponse(conv);
		return ;
	}
	conv.state = FINISH;
}
