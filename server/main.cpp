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
#include "server_application.hpp"

#include "SDL/SDL.h"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	cout << "Beginning server" << endl;
	try {
		ServerApplication app;
		app.Init(argc, argv);
		app.Loop();
		app.Quit();
	}
	catch(exception& e) {
		cerr << "Fatal error: " << e.what() << endl;
		return 1;
	}
	cout << "Clean exit" << endl;
	return 0;
}
/*/
#include "thread_safe_queue.hpp"

#include "SDL/SDL.h"

#include <iostream>

using namespace std;

struct Object {
	int value = 0;
};

int func(void* arg) {
	ThreadSafeQueue<Object>& queue = *reinterpret_cast<ThreadSafeQueue<Object>*>(arg);
	while(1) {
		Object o = queue.PopFront();
		if (o.value != 0) {
			cout << o.value;
			SDL_Delay(500);
			cout << endl;
		}
	}
}

int main(int, char**) {
	ThreadSafeQueue<Object> queue;

	SDL_Thread* thread1 = SDL_CreateThread(func, reinterpret_cast<void*>(&queue));
	SDL_Thread* thread2 = SDL_CreateThread(func, reinterpret_cast<void*>(&queue));
	SDL_Thread* thread3 = SDL_CreateThread(func, reinterpret_cast<void*>(&queue));

	while(1) {
		SDL_Delay(1000);
		Object o;
		o.value = 3;
		queue.PushBack(o);
	}
	return 0;
}
//*/