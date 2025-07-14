#include "CommentsRemover.hpp"

CommentsRemover::CommentsRemover(void) {}

CommentsRemover::~CommentsRemover(void) {}

std::string	CommentsRemover::removeComment(std::string& line, std::size_t operatorPos) {

	std::string	clearLine;

	clearLine = line.substr(0, operatorPos);
	clearLine += '\n';
	return (clearLine);
}

std::string	CommentsRemover::remove(std::string& configContent) {

	std::string	removedContent;
	std::string	currentLine;
	std::stringstream	rawContent;

	rawContent << configContent;
	while (getline(rawContent, currentLine)) {

		std::size_t	commentOperator = currentLine.find_first_of('#');

		if (commentOperator == std::string::npos) {
			removedContent += currentLine + '\n';
		}
		else {
			removedContent += removeComment(currentLine, commentOperator);
		}
	}
	return (removedContent);
}
