/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
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
