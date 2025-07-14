#pragma once
#include "../webserv.hpp"
#include <vector>

class Validator {
private:
	//Critical means we close the connexion
	void validateCritical(Conversation& conv);
	void validateVersion(Conversation& conv);
	void validateBodyFormat(Conversation& conv);

	//Worst case we skip the body
	void validateBenign(Conversation& conv);
	void validateUri(Conversation& conv);
	void segmentLinter(Conversation& conv, std::vector<std::string>& seg);
	void queryLinter(Conversation& conv);
	std::vector<std::string> parsePath(Conversation& conv);
	size_t matchLoc(Conversation& conv, std::vector<std::string>& seg);
	void assembleUri(Conversation& conv, std::vector<std::string>& seg, size_t match);
	void stripHost(Conversation& conv);
	void validateMethod(Conversation& conv);

	void validateHeader(Conversation& conv);
public:
	void execute(Conversation& conv);
};


