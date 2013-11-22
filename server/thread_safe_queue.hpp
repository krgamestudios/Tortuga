#ifndef THREADSAFEQUEUE_HPP_
#define THREADSAFEQUEUE_HPP_

#include "SDL/SDL_thread.h"

#include <deque>
#include <stdexcept>

/* This container is a thread safe reimplementation of std::queue.
*/

template<typename T, class Container = std::deque<T>>
class ThreadSafeQueue {
public:
	ThreadSafeQueue() {
		lock = SDL_CreateSemaphore(1);
		if (!lock) {
			throw(std::runtime_error("Failed to create ThreadSafeQueue::lock"));
		}
	}

	~ThreadSafeQueue() {
		SDL_DestroySemaphore(lock);
	}

	T PushBack(T t) {
		SDL_SemWait(lock);
		container.push_back(t);
		SDL_SemPost(lock);
		return t;
	}

	T PeekFront() {
		T t;
		SDL_SemWait(lock);
		if (container.size() > 0) {
			t = container[0];
		}
		SDL_SemPost(lock);
		return t;
	}

	T PopFront() {
		T t;
		SDL_SemWait(lock);
		if (container.size() > 0) {
			t = container[0];
			container.pop_front();
		}
		SDL_SemPost(lock);
		return t;
	}

	int Size() {
		//can't be sure if std::deque::size() is thread safe
		int ret;
		SDL_SemWait(lock);
		ret = container.size();
		SDL_SemPost(lock);
		return ret;
	}

private:
	Container container;
	SDL_sem* lock;
};

#endif
