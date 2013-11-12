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
#include "network_queue.hpp"

#include <stdexcept>

NetworkQueue::NetworkQueue() {
	lock = SDL_CreateSemaphore(1);
	if (!lock) {
		throw(std::runtime_error("Failed to create NetworkQueue::lock"));
	}
}

NetworkQueue::~NetworkQueue() {
	SDL_DestroySemaphore(lock);
}

NetworkPacket NetworkQueue::Push(NetworkPacket packet) {
	SDL_SemWait(lock);
	queue.push_back(packet);
	SDL_SemPost(lock);
	return packet;
}

NetworkPacket NetworkQueue::Peek() {
	NetworkPacket ret;
	SDL_SemWait(lock);
	if (queue.size() > 0) {
		ret = queue[0];
	}
	SDL_SemPost(lock);
	return ret;
}

NetworkPacket NetworkQueue::Pop() {
	NetworkPacket ret;
	SDL_SemWait(lock);
	if (queue.size() > 0) {
		ret = queue[0];
		queue.pop_front();
	}
	SDL_SemPost(lock);
	return ret;
}

void NetworkQueue::Flush() {
	SDL_SemWait(lock);
	queue.clear();
	SDL_SemPost(lock);
}

int NetworkQueue::Size() {
	//can't be sure if std::deque::size() is thread safe
	int ret;
	SDL_SemWait(lock);
	ret = queue.size();
	SDL_SemPost(lock);
	return ret;
}