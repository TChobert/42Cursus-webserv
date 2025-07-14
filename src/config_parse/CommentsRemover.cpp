#include "CommentsRemover.hpp"

CommentsRemover::CommentsRemover(void) {}

CommentsRemover::~CommentsRemover(void) {}

std::string	CommentsRemover::removeComment(const std::string& line, const std::size_t operatorPos) {

	std::string	clearLine;

	clearLine = line.substr(0, operatorPos);
	return (clearLine);
}

std::string	CommentsRemover::remove(const std::string& configContent) {

	std::string	removedContent;
	std::string	currentLine;
	std::stringstream	rawContent;

	rawContent << configContent;
	while (getline(rawContent, currentLine)) {

		std::size_t	commentOperator = currentLine.find_first_of(COMMENT_OPERATOR);

		if (commentOperator == std::string::npos) {
			removedContent += currentLine + '\n';
		}
		else if (commentOperator != 0) {
			removedContent += removeComment(currentLine, commentOperator) + '\n';
		}
	}

	return (removedContent);
}
