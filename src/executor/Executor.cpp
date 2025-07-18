#include "Executor.hpp"
#include "GetExecutor.hpp"
#include "PostExecutor.hpp"
#include "DeleteExecutor.hpp"

/* ---------------- PUBLIC METHODS ------------------ */

void	Executor::execute(Conversation& conv)
{
	switch (conv.eState)
	{
		case EXEC_START:
			Executor::executeStart(conv);
//cas pour GET
		case READ_EXEC_GET_STATIC:
			GetExecutor::resumeStatic(conv);
			break;
		case WRITE_EXEC_GET_AUTOINDEX:
			GetExecutor::resumeAutoIndex(conv);
			break;
		case READ_EXEC_GET_CGI:
			GetExecutor::resumeReadCGI(conv);
			break;
		case WRITE_EXEC_GET_CGI:
			GetExecutor::resumeWriteCGI(conv);
			break;
//cas pour POST
		case READ_EXEC_POST_BODY:
			PostExecutor::resumePostBody(conv);
			break;
		case WRITE_EXEC_POST_UPLOAD:
			PostExecutor::resumePostUpload(conv);
			break;
		case READ_EXEC_POST_CGI:
			PostExecutor::resumeReadPostCGI(conv);
			break;
		case WRITE_EXEC_POST_RESPONSE:
			PostExecutor::resumePostResponse(conv);
			break;
//cas pour DELETE
		case WRITE_EXEC_DELETE:
			DeleteExecutor::resumeDelete(conv);
			break;

		default:
			//ATTENTION: MAJ Struct Response?
			conv.resp.status = INTERNAL_SERVER_ERROR;
			break;
	}
}

void	Executor::executeStart(Conversation& conv)
{
	if (conv.resp.status != NOT_A_STATUS_CODE && conv.resp.status != CONTINUE)
		return;

	const std::string& method = conv.req.method;

	if (method == "GET")
		GetExecutor::handleGet(conv);
	else if (method == "POST")
		PostExecutor::handlePost(conv);
	else if (method == "DELETE")
		DeleteExecutor::handleDelete(conv);
	else
		//ATTENTION: MAJ Struct Response?
		conv.resp.status = NOT_IMPLEMENTED;
}
