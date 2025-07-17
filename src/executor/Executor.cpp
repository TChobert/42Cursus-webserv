#include "Executor.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

void	Executor::handleGet(Conversation& conv)
{

}

void	Executor::handlePost(Conversation& conv)
{

}

void	Executor::handleDelete(Conversation& conv)
{

}

/* ---------------- PUBLIC METHODS ------------------ */

void	Executor::execute(Conversation& conv)
{
	if (conv.resp.status != NOT_A_STATUS_CODE && conv.resp.status != CONTINUE)
		return;

	const std::string& method = conv.req.method;

	if (method == "GET")
		handleGet(conv);
	else if (method == "POST")
		handlePost(conv);
	else if (method == "DELETE")
		handleDelete(conv);
	else
		conv.resp.status = NOT_IMPLEMENTED;
}
