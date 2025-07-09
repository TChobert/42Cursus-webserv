#include "../../inc/webserv.hpp"

using namespace std;

void Validator::execute(Conversation& conv) {
	validateCritical(conv);
	if (conv.state != VALIDATE)
		return;
	validateBenign(conv);
}

void Validator::validateCritical(Conversation& conv) {
	validateVersion(conv);
	validateBodyFormat(conv);
}

void Validator::validateVersion(Conversation& conv) {
	if (conv.req.version != make_pair(1, 1))
		earlyResponse(conv, HTTP_VERSION_NOT_SUPPORTED);
}


void Validator::validateBodyFormat(Conversation& conv) {

