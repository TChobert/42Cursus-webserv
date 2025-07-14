#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <sstream>

class CommentsRemover {

	private:

	static const char COMMENT_OPERATOR = '#';
	std::string	removeComment(const std::string& line, const std::size_t operatorPos);

	public:

	CommentsRemover(void);
	~CommentsRemover(void);

	std::string	remove(const std::string& configContent);
};
