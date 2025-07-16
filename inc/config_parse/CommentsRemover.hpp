#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <sstream>

class CommentsRemover {

	private:

	static const std::string COMMENTS_OPERATORS;
	std::string	removeComment(const std::string& line);
	size_t	skipQuotes(const std::string& line, size_t position);

	public:

	CommentsRemover(void);
	~CommentsRemover(void);

	std::string	remove(const std::string& configContent);

	class UnclosedQuotesException: public std::exception {
		virtual const char *what() const throw();
	};
};
