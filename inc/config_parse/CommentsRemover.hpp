#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>

class CommentsRemover {

	public:

	CommentsRemover(void);
	~CommentsRemover(void);

	std::string	remove(std::string& configContent);
};
