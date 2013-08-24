#include "mail_box.hpp"

#include <sstream>
#include <stdexcept>

MailBox::MailBox() {
	if (!(inLock = SDL_CreateMutex())) {
		std::ostringstream os;
		os << "Failed to create a MailBox mutex; ";
		os << SDL_GetError();
		throw(std::runtime_error(os.str()));
	}
	if (!(outLock = SDL_CreateMutex())) {
		SDL_DestroyMutex(inLock);
		std::ostringstream os;
		os << "Failed to create a MailBox mutex; ";
		os << SDL_GetError();
		throw(std::runtime_error(os.str()));
	}
}

MailBox::~MailBox() {
	SDL_DestroyMutex(inLock);
	SDL_DestroyMutex(outLock);
}

std::string MailBox::PushIn(std::string msg) {
	SDL_LockMutex(inLock);
	input.push_back(msg);
	SDL_UnlockMutex(inLock);
	return msg;
}

std::string MailBox::PeekIn() {
	std::string ret;
	SDL_LockMutex(inLock);
	if (input.size()) {
		ret = input[0];
	}
	else {
		ret = "";
	}
	SDL_UnlockMutex(inLock);
	return ret;
}

std::string MailBox::PopIn() {
	std::string ret;
	SDL_LockMutex(inLock);
	if (input.size()) {
		ret = input[0];
		input.pop_front();
	}
	else {
		ret = "";
	}
	SDL_UnlockMutex(inLock);
	return ret;
}

std::string MailBox::PushOut(std::string msg) {
	SDL_LockMutex(outLock);
	output.push_back(msg);
	SDL_UnlockMutex(outLock);
	return msg;
}

std::string MailBox::PeekOut() {
	std::string ret;
	SDL_LockMutex(outLock);
	if (output.size()) {
		ret = output[0];
	}
	else {
		ret = "";
	}
	SDL_UnlockMutex(outLock);
	return ret;
}

std::string MailBox::PopOut() {
	std::string ret;
	SDL_LockMutex(outLock);
	if (output.size()) {
		ret = output[0];
		output.pop_front();
	}
	else {
		ret = "";
	}
	SDL_UnlockMutex(outLock);
	return ret;
}
