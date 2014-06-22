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

RoomData* RoomManager::CreateRoom(MapType mapType) {
	//create the room
	RoomData* newRoom = new RoomData();

	//create the generator, use a lambda because I'm lazy
	newRoom->generator = [mapType]() -> BaseGenerator* {
		switch(mapType) {
			case MapType::OVERWORLD: return new OverworldGenerator();
			case MapType::RUINS: return new RuinsGenerator();
			case MapType::TOWERS: return new TowersGenerator();
			case MapType::FORESTS: return new ForestsGenerator();
			case MapType::CAVES: return new CavesGenerator();
		}
		throw(std::runtime_error("Failed to set the room's generator"));
	}();

	//set the state
	if (luaState) {
		newRoom->pager.SetLuaState(luaState);
		newRoom->generator->SetLuaState(luaState);
	}

	//register the room
	roomMap[counter++] = newRoom;

	//API hook
	lua_getglobal(luaState, "Room");
	lua_getfield(luaState, -1, "OnCreate");
	lua_pushlightuserdata(luaState, newRoom);
	if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	lua_pop(luaState, 1);

	//finish the routine
	return newRoom;
}

void RoomManager::UnloadRoom(int uid) {
	//find the room
	RoomData* room = FindRoom(uid);
	if (!room) {
		return;
	}

	//API hook
	lua_getglobal(luaState, "Room");
	lua_getfield(luaState, -1, "OnUnload");
	lua_pushlightuserdata(luaState, room);
	if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	lua_pop(luaState, 1);

	//free the memory
	delete room->generator;
	delete room;
	roomMap.erase(uid);
}

RoomData* RoomManager::GetRoom(int uid) {
	RoomData* ptr = FindRoom(uid);
	if (ptr) return ptr;
	//TODO: proper Get() method
	return nullptr;
}

RoomData* RoomManager::FindRoom(int uid) {
	std::map<int, RoomData*>::iterator it = roomMap.find(uid);
	if (it == roomMap.end()) {
		return nullptr;
	}
	return it->second;
}

int RoomManager::PushRoom(RoomData* room) {
	roomMap[counter++] = room;
	return counter;
}

void RoomManager::UnloadAll() {
	lua_getglobal(luaState, "Room");

	for (auto& it : roomMap) {
		//API hook
		lua_getfield(luaState, -1, "OnUnload");
		lua_pushlightuserdata(luaState, it.second);
		if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
		}
	}

	lua_pop(luaState, 1);
	roomMap.clear();
}