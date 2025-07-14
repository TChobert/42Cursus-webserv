#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <sstream>

class CommentsRemover {

	private:

	static const char COMMENT_OPERATOR = '#';
	std::string	removeComment(const std::string& line);

	public:

	CommentsRemover(void);
	~CommentsRemover(void);

	std::string	remove(const std::string& configContent);

	class UnclosedQuotesException: public std::exception {
		virtual const char *what() const throw();
	};
};
