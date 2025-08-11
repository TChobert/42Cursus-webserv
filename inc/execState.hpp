#pragma once

enum execState
{
	EXEC_START,
	READ_EXEC_GET_CGI,
	READ_EXEC_POST_CGI,
	WRITE_EXEC_POST_CGI,
};
