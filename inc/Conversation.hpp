#include <string>
#include <cstdlib>
#include <map>
#include <sstream>

struct	ServerConfig;
class	RequestReader;
class	RequestParser;
class	RequestValidator;
class	Exec;
class	ResponseDesign;
class	ResponseSender;

typedef enum {
	CAN_READ,
	CAN_WRITE,
	CAN_TIMEOUT,
	NEED_READ,
	NEED_WRITE,
	DONE
} conversationStatus;

struct Conversation {

	int					fd;
	int					timeout;
	conversationStatus	status;
	ServerConfig&		config;

	RequestReader		read;
	RequestParser		parse;
	RequestValidator	validator;
	Exec				executor;
	ResponseDesign		designer;
	ResponseSender		sender;
};
