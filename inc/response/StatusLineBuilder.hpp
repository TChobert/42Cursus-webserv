#ifndef STATUSLINEBUILDER_HPP
# define STATUSLINEBUILDER_HPP

# include <string>
# include <map>
# include "webserv.hpp"

class Response;

class StatusLineBuilder
{
	private:
		static const std::map<int, std::string> _statusTexts;

		//fonction utilitaire pour init la map au-dessus
		static std::map<int, std::string> createStatusTexts();

	public:
		static std::string build(const response& resp);
		static std::string getStatusText(int code);
};

#endif
