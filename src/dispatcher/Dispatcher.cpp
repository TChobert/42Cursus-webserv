#include "Dispatcher.hpp"

Dispatcher::Dispatcher(int& EpollFd, std::map<int, Conversation>& executorFds, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender) :
		_epollFd(EpollFd), _reader(reader), _parser(parser), _validator(validator), _executor(executor),
		_responseBuilder(responseBuilder), _sender(sender), _postSender(postSender), _executorFds(executorFds) {}

Dispatcher::~Dispatcher(void) {}

bool	Dispatcher::isInExecutorFdsMap(const int& fd) const {

	std::map<int, Conversation>::const_iterator it;
	for (it = _executorFds.begin(); it != _executorFds.end(); ++it) {
		return (it->first == fd);
	}
	return (false);
}

void	Dispatcher::setExecutorInterest(Conversation& conv, e_interest_mode mode) {

	struct epoll_event ev;
	ev.data.fd = conv.tempFd;

	switch (mode) {
		case READ_EXEC_FD:
			ev.events = EPOLLIN;
			break;
		case WRITE_EXEC_FD:
			ev.events = EPOLLOUT;
			break;
	}

	if (!isInExecutorFdsMap(conv.tempFd)) {
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, conv.tempFd, &ev) < 0) {
			close(conv.tempFd);
			std::ostringstream oss;
			oss << "Failed to add fd requested by executor (" << conv.tempFd << ") to interest list. Closing it.";
			throw std::runtime_error(oss.str());
		}
		_executorFds[conv.tempFd]= conv;
	}
	else {
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.tempFd, &ev) < 0) {
			std::cerr << "epoll_ctl MOD failed on fd " << conv.tempFd << ": " << strerror(errno) << std::endl;
			throw std::runtime_error("Failed to modify fd in epoll");
		}
	}
}

void	Dispatcher::setClientInterest(Conversation& conv, e_interest_mode mode) {

	struct epoll_event ev;

	ev.data.fd = conv.fd;
	switch (mode) {
		case READ:
			ev.events = EPOLLIN;
			break ;
		case WRITE:
			ev.events = EPOLLOUT;
			break;
	}
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.fd, &ev) < 0) {
		std::cerr << "Failed to change interest mode on fd: " << conv.fd << std::endl;
	}
}

void	Dispatcher::setEpollInterest(Conversation& conv, e_interest_mode mode) {

	struct epoll_event	ev;

	switch (mode) {
		case READ:
		case WRITE :
			setClientInterest(conv, mode);
			break;
		case READ_EXEC_FD:
		case WRITE_EXEC_FD:
			setExecutorInterest(conv, mode);
			break;
	}
}

void	Dispatcher::removeClientFromEpoll(Conversation& conv) {

	int	clientFd = conv.fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL) < 0) {
		std::cerr << "Failed to remove fd " << conv.config.identity.host << " from epoll\n";
	}
}

void	Dispatcher::dispatch(Conversation& conv) {

	if (conv.state == WRITE_CLIENT)
		conv.state = TO_SEND;
	else if (conv.state == READ_CLIENT)
		conv.state = TO_READ;
	else if (conv.state == READ_EXEC)
		conv.state = EXEC;
	else if (conv.state == WRITE_EXEC)
		conv.state = EXEC;

	while (true) {

		if (conv.state == TO_SEND) {
			_sender->execute(conv);
		}
		else if (conv.state == EXEC) {
			_executor->execute(conv);
		}
		else if (conv.state == TO_READ) {
			_reader->execute(conv);
		}
		else if (conv.state == RESPONSE) {
			_responseBuilder->execute(conv);
		}
		else if (conv.state == IS_SENT) {
			_postSender->execute(conv);
		}
		else if (conv.state == PARSE || PARSE_BODY || EOF_CLIENT) {
			_parser->execute(conv);
		}
		else if (conv.state == VALIDATE) {
			_validator->execute(conv);
		}
		else if (conv.state == READ_EXEC) {
			setEpollInterest(conv, READ_EXEC_FD);
			break ;
		}
		else if (conv.state == WRITE_EXEC) {
			setEpollInterest(conv, WRITE_EXEC_FD);
			break ;
		}
		else if (conv.state == READ_CLIENT) {
			setEpollInterest(conv, READ);
			break ;
		}
		else if (conv.state == WRITE_CLIENT) {
			setEpollInterest(conv, WRITE);
			break ;
		}
		else if (conv.state == FINISH) {
			removeClientFromEpoll(conv);
			break ;
		}
	}
}
