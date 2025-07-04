#ifndef STATUSLINEBUILDER_HPP
# define STATUSLINEBUILDER_HPP

# include <string>
# include <map>
# include "HttpResponse.hpp"

class HttpResponse;

class StatusLineBuilder
{
	public:
		static std::string build(const HttpResponse& response);

	private:
		static const std::map<int, std::string> _statusTexts;

		//fonction utilitaire pour init la map au-dessus
		static std::map<int, std::string> createStatusTexts();
};

#endif
