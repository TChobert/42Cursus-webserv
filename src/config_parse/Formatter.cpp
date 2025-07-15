#include "Formatter.hpp"

Formatter::Formatter(void) {}

Formatter::~Formatter(void) {}

const std::string Formatter::SPACES =  " 	";

bool Formatter::isEmptyLine(const std::string& line) const {

	for (size_t i = 0; i < line.length(); ++i) {
		if (!isspace(static_cast<unsigned char>(line[i])))
			return (false);
	}
	return (true);
}

std::string Formatter::removeSpaces(const std::string& content) {

	std::stringstream rawContent(content);
	std::string result;
	std::string currentLine;

	while (getline(rawContent, currentLine)) {

	size_t firstNonSpace = 0;
	while (firstNonSpace < currentLine.size() && isspace(static_cast<unsigned char>(currentLine[firstNonSpace]))) {
		++firstNonSpace;
	}
		result += currentLine.substr(firstNonSpace, currentLine.length()) + '\n';
	}
	return (result);
}

std::string Formatter::removeEmptyLines(const std::string& content) {

	std::stringstream rawContent(content);
	std::string emptyLinesRemoved;
	std::string currentLine;

	while (getline(rawContent, currentLine)) {

		if (isEmptyLine(currentLine) == false) {
			emptyLinesRemoved += currentLine + '\n';
		}
	}
	return (emptyLinesRemoved);
}

std::vector<std::string> Formatter::splitContent(const std::string& content) {

	std::vector<std::string> splittedContent;
	std::stringstream contentStream(content);
	std::string currentLine;

	while (getline(contentStream, currentLine)) {
		splittedContent.push_back(currentLine);
	}
	return (splittedContent);
}

std::vector<std::string> Formatter::format(const std::string& content) {

	std::vector<std::string> formatted;
	
	std::string spacesRemoved = removeSpaces(content);
	std::string emptyLinesRemoved = removeEmptyLines(spacesRemoved);
	formatted = splitContent(emptyLinesRemoved);

	return (formatted);
}
