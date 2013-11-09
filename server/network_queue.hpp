#ifndef NETWORKQUEUE_HPP_
#define NETWORKQUEUE_HPP_

#include "udp_network_utility.hpp"
#include "network_packet.hpp"

#include "SDL/SDL_thread.h"

#include <deque>

class NetworkQueue {
public:
	NetworkQueue() = default;
	~NetworkQueue() = default;

	void Init(UDPNetworkUtility*);
	void Quit();
	void Kill();

	NetworkPacket Peek();
	NetworkPacket Pop();
	void Flush();

	int Size();
private:
	friend int networkQueueThread(void*);
	void ResetMembers();

	bool running = false;

	UDPNetworkUtility* netUtil = nullptr;
	SDL_sem* lock = nullptr;
	SDL_Thread* thread = nullptr;
	std::deque<NetworkPacket> queue;
};

#endif
