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
#include "service_locator.hpp"

#include "udp_network_utility.hpp"

#include "SDL/SDL_thread.h"

#include <stdexcept>
#include <deque>

static SDL_sem* lock = SDL_CreateSemaphore(1);
static SDL_Thread* queueThread = nullptr;

static std::deque<Packet> queue;

static bool running = false;

static int networkQueue(void*) {
	UDPNetworkUtility* netUtil = ServiceLocator<UDPNetworkUtility>::Get();
	while(running) {
		SDL_SemWait(lock);
		while(netUtil->Receive()) {
			Packet p;
			memcpy(&p, netUtil->GetInData(), sizeof(Packet));
			p.meta.address = netUtil->GetInPacket()->address;
			queue.push_back(p);
		}
		SDL_SemPost(lock);
		SDL_Delay(10);
	}
	return 0;
}

void beginQueueThread() {
	if (running) {
		return;
	}
	running = true;
	if (!(queueThread = SDL_CreateThread(networkQueue, nullptr))) {
		throw(std::runtime_error("Failed to create the network thread"));
	}
}

void endQueueThread() {
	if (!running) {
		return;
	}
	running = false;
	SDL_WaitThread(queueThread, nullptr);
	queueThread = nullptr;
}

void killQueueThread() {
	if (!running) {
		return;
	}
	running = false;
	SDL_KillThread(queueThread);
	queueThread = nullptr;
}

Packet peekNetworkPacket() {
	SDL_SemWait(lock);
	Packet p;
	if (queue.size() > 0) {
		Packet p = queue[0];
	}
	SDL_SemPost(lock);
	return p;
}

Packet popNetworkPacket() {
	SDL_SemWait(lock);
	Packet p;
	if (queue.size() > 0) {
		p = queue[0];
		queue.pop_front();
	}
	SDL_SemPost(lock);
	return p;
}

void flushNetworkQueue() {
	UDPNetworkUtility* netUtil = ServiceLocator<UDPNetworkUtility>::Get();
	SDL_SemWait(lock);
	while(netUtil->Receive());
	queue.clear();
	SDL_SemPost(lock);
}