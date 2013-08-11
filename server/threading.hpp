#ifndef THREADING_HPP_
#define THREADING_HPP_

#include "room.hpp"

#include "SDL/SDL_thread.h"

struct roomHandle {
	SDL_Thread* thread = nullptr;
	Room* room = nullptr;
};

int roomThread(void*);

#endif
