#ifndef NETWORKQUEUE_HPP_
#define NETWORKQUEUE_HPP_

#include "network_packet.hpp"

#include "SDL/SDL_thread.h"

#include <deque>

class NetworkQueue {
public:
	NetworkQueue();
	~NetworkQueue();

	NetworkPacket Push(NetworkPacket);
	NetworkPacket Peek();
	NetworkPacket Pop();
	void Flush();

	int Size();

	SDL_sem* GetLock() const { return lock; }
private:
	std::deque<NetworkPacket> queue;
	SDL_sem* lock;
};

#endif
