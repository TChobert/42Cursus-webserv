#include "CommentsRemover.hpp"

const std::string CommentsRemover::COMMENTS_OPERATORS = "#;";

CommentsRemover::CommentsRemover(void) {}

CommentsRemover::~CommentsRemover(void) {}

size_t	CommentsRemover::skipQuotes(const std::string& line, size_t position) {

	size_t closingQuote = line.find('"', position + 1);
	if (closingQuote == std::string::npos) {
		throw UnclosedQuotesException();
	}
	position = closingQuote;
	return (position);
}

std::string	CommentsRemover::removeComment(const std::string& line) {

	std::string	clearLine;

	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] == '"') {
			i = skipQuotes(line, i);
			continue ;
		}
		if (COMMENTS_OPERATORS.find_first_of(line[i]) != std::string::npos) {
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

		std::string cleaned = removeComment(currentLine);
		removedContent += cleaned + '\n';
	}

	return (removedContent);
}

// EXCEPTIONS

const char	*CommentsRemover::UnclosedQuotesException::what() const throw() {

	return ("Error: webserv: unclosed quotes in configuration file.");
}
