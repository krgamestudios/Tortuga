#include "network_queue.hpp"

#include "SDL/SDL_thread.h"

#include <deque>

static std::deque<Packet> queue;

int networkQueue(void*) {
	//
}

Packet getPacket() {
	//
}