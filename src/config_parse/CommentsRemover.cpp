#include "CommentsRemover.hpp"

CommentsRemover::CommentsRemover(void) {}

CommentsRemover::~CommentsRemover(void) {}

// std::string	CommentsRemover::removeComment(const std::string& line) {

// 	std::string	clearLine;
// 	bool	isInQuotes = false;

// 	for (size_t i = 0; i < line.length(); ++i) {
// 		if (line[i] == '"')
// 			isInQuotes = !isInQuotes;
// 		if (line[i] == COMMENT_OPERATOR && isInQuotes == false) {
// 			clearLine = line.substr(0, i);
// 			break ;
// 		}
// 	}
// 	return (clearLine);
// }

std::string	CommentsRemover::removeComment(const std::string& line) {

	std::string	clearLine;

	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] == '"') {
			size_t closingQuote = line.find('"', i + 1);
			if (closingQuote == std::string::npos) {
				throw UnclosedQuotesException();
			}
			i = closingQuote;
			continue ;
		}
		if (line[i] == COMMENT_OPERATOR) {
			clearLine = line.substr(0, i);
			return clearLine;
		}
	}
	clearLine = line;
	return clearLine;
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
			removedContent += removeComment(currentLine) + '\n';
		}
	}

	return (removedContent);
}

// EXCEPTIONS

const char	*CommentsRemover::UnclosedQuotesException::what() const throw() {

	return ("Error: webserv: unclosed quotes in configuration file.");
}
