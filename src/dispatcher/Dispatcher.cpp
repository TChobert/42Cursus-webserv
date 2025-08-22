#include "Dispatcher.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include<fcntl.h>

Dispatcher::Dispatcher(int& EpollFd,
						std::map<int, Conversation>& clientsFds,
						std::map<int, Conversation*>& executorFds,
						IModule* reader,
						IModule* parser,
						IModule* validator,
						IModule* executor,
						IModule* responseBuilder,
						IModule* sender,
						IModule* postSender) :
	_epollFd(EpollFd),
	_clientsFds(clientsFds),
	_executorFds(executorFds),
	_reader(reader),
	_parser(parser),
	_validator(validator),
	_executor(executor),
	_responseBuilder(responseBuilder),
	_sender(sender),
	_postSender(postSender) {}

Dispatcher::~Dispatcher(void) {}

bool	Dispatcher::isInExecutorFdsMap(const int& fd) const {

	return _executorFds.find(fd) != _executorFds.end();
}

void	Dispatcher::setExecutorInterest(Conversation& conv, e_interest_mode mode) {

	if (conv.cgiIn < 0) {
		std::cerr << "Invalid tempFd passed to setExecutorInterest" << std::endl;
		return ;
	}
	struct epoll_event ev;
	ev.data.fd = conv.cgiIn;

	switch (mode) {
		case READ_EXEC_FD:
			ev.events = EPOLLIN;
			break;
		case WRITE_EXEC_FD:
			ev.events = EPOLLOUT;
			break;
	}
	if (!isInExecutorFdsMap(conv.cgiIn)) {
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, conv.cgiIn, &ev) < 0) {
			std::cerr << "Failed to add fd requested by executor (" << conv.cgiIn << ") to interest list. Closing it." << std::endl;
			close(conv.cgiIn);
			return ;
		}
		_executorFds[conv.cgiIn]= &conv;
	}
	else {
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.cgiIn, &ev) < 0) {
			std::cerr << "epoll_ctl MOD failed on fd " << conv.cgiIn << ": " << strerror(errno) << ". Closing it." << std::endl;
			close(conv.cgiIn);
			_executorFds.erase(conv.cgiIn);
			return ;
		}
	}
}

void Dispatcher::setClientInterest(Conversation& conv, e_interest_mode mode) {
	std::cout << "=== DEBUG setClientInterest ===" << std::endl;
	std::cout << "fd: " << conv.fd << ", mode: " << mode << std::endl;
	std::cout << "_epollFd: " << _epollFd << std::endl;

	// Vérifier si le fd est encore valide
	int flags = fcntl(conv.fd, F_GETFL);
	if (flags == -1) {
		std::cerr << "ERROR: fd " << conv.fd << " is closed! " << strerror(errno) << std::endl;
		return;
	}
	std::cout << "fd " << conv.fd << " is still open (flags: " << flags << ")" << std::endl;

	// Vérifier si _epollFd est valide
	flags = fcntl(_epollFd, F_GETFL);
	if (flags == -1) {
		std::cerr << "ERROR: _epollFd " << _epollFd << " is invalid! " << strerror(errno) << std::endl;
		return;
	}

	if (_clientsFds.find(conv.fd) == _clientsFds.end()) {
		std::cerr << "Client fd not found in clients map: " << conv.fd << std::endl;
		return ;
	}

	struct epoll_event ev;
	ev.data.fd = conv.fd;

	switch (mode) {
	case READ:
		ev.events = EPOLLIN;
		std::cout << "Setting EPOLLIN (" << EPOLLIN << ")" << std::endl;
		break;
	case WRITE:
		ev.events = EPOLLOUT;
		std::cout << "Setting EPOLLOUT (" << EPOLLOUT << ")" << std::endl;
		break;
	default:
		std::cerr << "Unknown mode in setClientInterest: " << mode << std::endl;
		return;
	}

	std::cout << "Calling epoll_ctl with: epollfd=" << _epollFd
			<< ", fd=" << conv.fd << ", events=" << ev.events << std::endl;

	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.fd, &ev) < 0) {
		std::cout << "MOD failed: " << strerror(errno) << ", trying ADD..." << std::endl;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, conv.fd, &ev) < 0) {
			std::cerr << "ADD also failed: " << strerror(errno) << std::endl;
		} else {
			std::cout << "ADD successful!" << std::endl;
		}
	} else {
		std::cout << "MOD successful!" << std::endl;
	}
	std::cout << "=== END DEBUG ===" << std::endl;
}

// void Dispatcher::setClientInterest(Conversation& conv, e_interest_mode mode) {

// 	if (_clientsFds.find(conv.fd) == _clientsFds.end()) {
// 		std::cerr << "Client fd not found in clients map: " << conv.fd << std::endl;
// 		return ;
// 	}
// 	struct epoll_event ev;
// 	ev.data.fd = conv.fd;

// 	switch (mode) {
// 		case READ:
// 			ev.events = EPOLLIN;
// 			break;
// 		case WRITE:
// 			ev.events = EPOLLOUT;
// 			break;
// 		default:
// 		std::cerr << "Unknown mode in setClientInterest\n";
// 		return;
// 	}
// 	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.fd, &ev) < 0) {
// 		std::cerr << "Failed to change interest mode on fd: " << conv.fd << std::endl;
// 	}
// }

void	Dispatcher::setEpollInterest(Conversation& conv, e_interest_mode mode) {

	switch (mode) {
		case READ:
		case WRITE:
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

// void	Dispatcher::removeExecutorFdFromEpoll(Conversation& conv) {

// 	int executorFd = conv.fdToClose;

// 	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, executorFd, NULL) < 0) {
// 		std::cerr << "Failed to remove executor fd " << conv.config.identity.host << " from epoll\n";
// 	}
// 	close(executorFd);
// 	_executorFds.erase(conv.fdToClose);
// 	conv.fdToClose = -1;
// }

void Dispatcher::setTimeoutResponse(Conversation& conv) {

	return earlyResponse(conv, GATEWAY_TIMEOUT);
}

void	Dispatcher::dispatch(Conversation& conv) {

	std::cout << "Dispatcher called" << std::endl;
	if (conv.state == WRITE_CLIENT)
		conv.state = TO_SEND;
	else if (conv.state == READ_CLIENT)
		conv.state = TO_READ;
	else if (conv.state == READ_EXEC)
		conv.state = EXEC;
	else if (conv.state == WRITE_EXEC)
		conv.state = EXEC;

	while (true) {

		if (conv.state == CLIENT_TIMEOUT) {
			setTimeoutResponse(conv);
		}
		else if (conv.state == TO_SEND) {
			_sender->execute(conv);
		}
		else if (conv.state == EXEC) {
			_executor->execute(conv);
		}
		else if (conv.state == TO_READ) {
			std::cout << "READER CALLED" << std::endl;
			_reader->execute(conv);
		}
		else if (conv.state == RESPONSE) {
			_responseBuilder->execute(conv);
		}
		else if (conv.state == IS_SENT) {
			_postSender->execute(conv);
		}
		else if (conv.state == PARSE || conv.state == PARSE_BODY || conv.state == EOF_CLIENT) {
			std::cout << "PARSER CALLED" << std::endl;
			_parser->execute(conv);
		}
		else if (conv.state == VALIDATE) {
			std::cout << "VALIDATOR CALLED" << std::endl;
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

	// if (conv.fdToClose != -1) {
	// 	removeExecutorFdFromEpoll(conv);
	// }
}
