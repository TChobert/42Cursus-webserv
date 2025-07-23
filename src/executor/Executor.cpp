#include <fstream>
#include <sstream>
#include "Executor.hpp"
#include "GetExecutor.hpp"
#include "PostExecutor.hpp"
#include "DeleteExecutor.hpp"
#include "webserv_utils.hpp"

/* ---------------- PUBLIC METHODS ------------------ */

void	Executor::updateResponseData(Conversation& conv)
{
	if (conv.resp.status >= 400)
	{
		std::map<int, std::string>& errorPages = conv.config.errorPagesCodes;
		std::map<int, std::string>::iterator it = errorPages.find(conv.resp.status);

		if (it != errorPages.end())
		{
			const std::string& errorPagePath = it->second;

			std::ifstream file;
			file.open(errorPagePath.c_str());

			if (file.is_open())
			{
				std::ostringstream ss;
				ss << file.rdbuf();
				conv.resp.body = ss.str();
				conv.resp.contentType = getMimeType(errorPagePath);
			}
			else
			{
				conv.resp.body = "<html><body><h1>" + intToString(conv.resp.status) + " Error</h1></body></html>";
				conv.resp.contentType = "text/html";
			}
		}
	}

	if ((conv.resp.status == CREATED || (conv.resp.status >= 300 && conv.resp.status < 400))
		&& conv.resp.location.empty())
	{
		//MAJ avec la location nouvellement creee
		//TODO vis a vis de POST
	}

	if (conv.resp.contentType.empty())
	{
		if (conv.resp.body.find("<html>") != std::string::npos)
		{
			conv.resp.contentType = "text/html";
		}
		else
		{
			std::string type = getMimeType(conv.req.pathOnDisk);
			if (type == "application/octet-stream")
				type = "text/plain";
			conv.resp.contentType = type;
		}
	}
}

void	Executor::setResponse(Conversation& conv, statusCode code)
{
	conv.resp.status = code;
	updateResponseData(conv);
	conv.state = RESPONSE;
}

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
// 		case READ_EXEC_GET_CGI:
// 			GetExecutor::resumeReadCGI(conv);
// 			break;
// 		case WRITE_EXEC_GET_CGI:
// 			GetExecutor::resumeWriteCGI(conv);
// 			break;
//cas pour POST
		case WRITE_EXEC_POST_BODY:
			PostExecutor::resumePostWriteBodyToFile(conv);
			break;
// 		case READ_EXEC_POST_CGI:
// 			PostExecutor::resumeReadPostCGI(conv);
// 			break;
// 		case WRITE_EXEC_POST_RESPONSE:
// 			PostExecutor::resumePostResponse(conv);
// 			break;
// //cas pour DELETE
// 		case WRITE_EXEC_DELETE:
// 			DeleteExecutor::resumeDelete(conv);
// 			break;
		default:
			setResponse(conv, INTERNAL_SERVER_ERROR);
			break;
	}
}

void	Executor::executeStart(Conversation& conv)
{
	if (conv.resp.status != NOT_A_STATUS_CODE && conv.resp.status != CONTINUE)
	{
		setResponse(conv, conv.resp.status);
		return;
	}

	const std::string& method = conv.req.method;
	if (method == "GET")
		GetExecutor::handleGet(conv);
	else if (method == "POST")
		PostExecutor::handlePost(conv);
	else if (method == "DELETE")
		DeleteExecutor::handleDelete(conv);
	else
		setResponse(conv, NOT_IMPLEMENTED);
}
