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
		SDL_SemWait(lock);
		container.clear();
		SDL_SemPost(lock);
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
