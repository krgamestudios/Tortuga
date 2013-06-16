#include "network_queue.hpp"
#include "service_locator.hpp"

#include "udp_network_utility.hpp"

#include "SDL/SDL_thread.h"

#include <deque>

static SDL_sem* lock = SDL_CreateSemaphore(1);

static std::deque<Packet> queue;

int networkQueue(void*) {
	UDPNetworkUtility* netUtil = ServiceLocator<UDPNetworkUtility>::Get();
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