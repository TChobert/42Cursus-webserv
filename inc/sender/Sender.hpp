#ifndef SENDER_HPP
# define SENDER_HPP

class Conversation;

class Sender
{
	public:
		static void sendResponse(Conversation& conv);
};

#endif
