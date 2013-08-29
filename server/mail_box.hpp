#ifndef MAILBOX_HPP_
#define MAILBOX_HPP_

#include "SDL/SDL_thread.h"

#include <deque>
#include <string>

//Thread safe messaging system
class MailBox {
public:
	MailBox();
	~MailBox();

	std::string PushIn(std::string);
	std::string PeekIn();
	std::string PopIn();
	std::string PushOut(std::string);
	std::string PeekOut();
	std::string PopOut();
private:
	std::deque<std::string> input;
	std::deque<std::string> output;
	SDL_mutex* inLock = nullptr;
	SDL_mutex* outLock = nullptr;
};

#endif
