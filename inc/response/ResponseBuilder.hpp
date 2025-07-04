#ifndef RESPONSEBUILDER_HPP
# define RESPONSEBUILDER_HPP

# include <string>
# include "StatusLineBuilder.hpp"
# include "HeaderBuilder.hpp"
# include "BodyBuilder.hpp"
# include "ResponseAssembler.hpp"

class Conversation;

class ResponseBuilder
{
	public:
		std::string build(Conversation& conv);

	private:

};

#endif
