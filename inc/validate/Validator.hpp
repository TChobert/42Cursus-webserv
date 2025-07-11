#pragma once
#include "../webserv.hpp"

class Validator {
private:
	//Critical means we close the connexion
	void validateCritical(Conversation& conv);
	void validateVersion(Conversation& conv);
	void validateBodyFormat(Conversation& conv);

	//Worst case we skip the body
	void validateBenign(Conversation& conv);
	void validateUri(Conversation& conv);
	size_t matchLoc(Conversation& conv);
	void stripHost(Conversation& conv);
	void validateMethod(Conversation& conv);

	void validateHeader(Conversation& conv);
public:
	void execute(Conversation& conv);
};


