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