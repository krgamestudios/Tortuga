#include "network_queue.hpp"
#include "service_locator.hpp"

#include "udp_network_utility.hpp"

#include "SDL/SDL_thread.h"

#include <deque>

#ifdef DEBUG
#include <iostream>
#endif

static SDL_sem* lock = SDL_CreateSemaphore(1);

static std::deque<Packet> queue;

int networkQueue(void*) {
	UDPNetworkUtility* netUtil = ServiceLocator<UDPNetworkUtility>::Get();
#ifdef DEBUG
	std::cout << "int networkQueue(void*) active" << std::endl;
#endif
	for(;;) {
		SDL_SemWait(lock);
		while(netUtil->Receive()) {
			Packet p;
			memcpy(&p, netUtil->GetInData(), sizeof(Packet));
			queue.push_back(p);
		}
		SDL_SemPost(lock);
		SDL_Delay(10);
	}
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
		Packet p = queue[0];
		queue.pop_front();
	}
	SDL_SemPost(lock);
	return p;
}