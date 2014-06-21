/* Copyright: (c) Kayne Ruse 2014
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
#include "room_manager.hpp"

//the generator types
#include "overworld_generator.hpp"
#include "ruins_generator.hpp"
#include "towers_generator.hpp"
#include "forests_generator.hpp"
#include "caves_generator.hpp"

#include <stdexcept>

//-------------------------
//public access methods
//-------------------------

int RoomManager::CreateRoom(MapType mapType) {
	//create the room
	RoomData* newRoom = new RoomData();

	//set the state
	if (luaState) {
		newRoom->pager.SetLuaState(luaState);
	}

	//create the generator
	newRoom->generator = [mapType]() -> BaseGenerator* {
		switch(mapType) {
			case MapType::NONE: //use overworld as a default
			case MapType::OVERWORLD: return new OverworldGenerator();
			case MapType::RUINS: return new RuinsGenerator();
			case MapType::TOWERS: return new TowersGenerator();
			case MapType::FORESTS: return new ForestsGenerator();
			case MapType::CAVES: return new CavesGenerator();
		}
		throw(std::runtime_error("Failed to set the room's generator"));
	}();

	//finish the routine
	roomMap[counter] = newRoom;
	return counter++;
}

int RoomManager::UnloadRoom(int uid) {
	RoomData* room = FindRoom(uid);
	if (!room) {
		return -1;
	}

	delete room->generator;
	delete room;
	roomMap.erase(uid);

	return 0;
}

RoomData* RoomManager::GetRoom(int uid) {
	RoomData* ptr = FindRoom(uid);
	if (ptr) return ptr;
	int newIndex = CreateRoom(MapType::NONE);
	return FindRoom(newIndex);
}

RoomData* RoomManager::FindRoom(int uid) {
	std::map<int, RoomData*>::iterator it = roomMap.find(uid);
	if (it == roomMap.end()) {
		return nullptr;
	}
	return it->second;
}

int RoomManager::PushRoom(RoomData* room) {
	roomMap[counter] = room;
	return counter++;
}
