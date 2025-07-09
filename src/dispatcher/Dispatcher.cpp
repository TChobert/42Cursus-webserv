#include "Dispatcher.hpp"

Dispatcher::Dispatcher(int& EpollFd, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender) :
		_epollFd(EpollFd), _reader(reader), _parser(parser), _validator(validator), _executor(executor),
		_responseBuilder(responseBuilder), _sender(sender), _postSender(postSender) {}

Dispatcher::~Dispatcher(void) {}

void	Dispatcher::dispatch(Conversation& conv) {
}
