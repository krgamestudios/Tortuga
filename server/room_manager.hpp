#ifndef ROOMMANAGER_HPP_
#define ROOMMANAGER_HPP_

#include "base_room.hpp"

#include "SDL/SDL_thread.h"

#include <list>

class RoomManager {
private:
	RoomManager() = default;
	~RoomManager() = default;
	static RoomManager instance;

public:
	static RoomManager* GetInstance() { return &instance; }

	//open room
	//close room

	//get room
	//set?

private:
	struct RoomHandle {
		SDL_Thread* thread = nullptr;
		BaseRoom* room = nullptr;
	};
	std::list<RoomHandle> rooms;
};

#endif
