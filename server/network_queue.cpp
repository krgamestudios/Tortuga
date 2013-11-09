#include "network_queue.hpp"

#include "utility.hpp"

#include <stdexcept>
#include <iostream>

int networkQueueThread(void* ptr) {
	NetworkQueue* netQueue = reinterpret_cast<NetworkQueue*>(ptr);
	NetworkPacket packet;

	while(netQueue->running) {
		SDL_SemWait(netQueue->lock);

		//suck in the waiting packets
		while(netQueue->netUtil->Receive()) {
			memcpy(&packet, netQueue->netUtil->GetInData(), sizeof(NetworkPacket));
			//this is important: keep track of the source address
			packet.meta.srcAddress = netQueue->netUtil->GetInPacket()->address;
			netQueue->queue.push_back(packet);
		}

		SDL_SemPost(netQueue->lock);
		SDL_Delay(10);
	}
	return 0;
}

void NetworkQueue::Init(UDPNetworkUtility* ptr) {
	if (running) {
		throw(std::runtime_error("Network Queue is already running"));
	}
	running = true;
	netUtil = ptr;
	lock = SDL_CreateSemaphore(1);
	thread = SDL_CreateThread(networkQueueThread, this);
	if (!thread) {
		throw(std::runtime_error("Failed to create the network thread"));
	}
}

void NetworkQueue::Quit() {
	if (!running) {
		return;
	}

	//end the thread
	running = false;
	int ret;
	SDL_WaitThread(thread, &ret);

	ResetMembers();

	//handle the return
	if (ret != 0) {
		throw(std::runtime_error(std::string() + "Network thread returned error code: " + to_string_custom(ret)));
	}
}

void NetworkQueue::Kill() {
	if (!running) {
		return;
	}

	running = false;
	SDL_KillThread(thread);

	ResetMembers();
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
	while(netUtil->Receive());
	queue.clear();
	SDL_SemPost(lock);
}

void NetworkQueue::ResetMembers() {
	if (running) {
		throw(std::logic_error("Cannon reset the queue while running"));
	}

	//reset
	netUtil = nullptr;
	SDL_DestroySemaphore(lock);
	lock = nullptr;
	thread = nullptr;
	queue.clear();
}

int NetworkQueue::Size() {
	//can't be sure if std::deque::size() is thread safe
	int ret;
	SDL_SemWait(lock);
	ret = queue.size();
	SDL_SemPost(lock);
	return ret;
}