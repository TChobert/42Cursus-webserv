#include "Executor.hpp"
#include "GetExecutor.hpp"
#include "PostExecutor.hpp"
#include "DeleteExecutor.hpp"
#include "webserv_utils.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

void	Executor::updateResponseData(Conversation& conv)
{
	//content-type encore a creuser -- updates en fonction des erreurs...
	if (conv.resp.contentType.empty())
		conv.resp.contentType = getMimeType(conv.req.uri); //a revoir d'ou je recupere l'extension..
		//ex: 404.html est pathOnDisk: est-ce stocke avant dans conv.req.uri ou pas? donc pas forcement conv.req.uri..

	if ((conv.resp.status == CREATED || (conv.resp.status >= 300 && conv.resp.status < 400))
		&& conv.resp.location.empty())
	{
		conv.resp.location = conv.req.uri;
	}

	// gerer cookies aussi ?
}


/* ---------------- PUBLIC METHODS ------------------ */

void	Executor::execute(Conversation& conv)
{
	switch (conv.eState)
	{
		case EXEC_START:
			Executor::executeStart(conv);
			break;
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
	{
		updateResponseData(conv);
		conv.resp.status = NOT_IMPLEMENTED;
	}
}
