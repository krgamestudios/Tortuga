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
