#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <sstream>

class CommentsRemover {

	private:

	std::string	removeComment(std::string& line, std::size_t operatorPos);

	public:

	CommentsRemover(void);
	~CommentsRemover(void);

	std::string	remove(std::string& configContent);
};
