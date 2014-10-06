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

#include "room_api.hpp"

#include <stdexcept>

//-------------------------
//public access methods
//-------------------------

int RoomManager::Create() {
/*	//create the room
	RoomData* newRoom = 
	newRoom->pager.SetLuaState(luaState);

	//register the room
	roomMap[counter] = newRoom;

	//API hook
	lua_getglobal(luaState, TORTUGA_ROOM_NAME);
	lua_getfield(luaState, -1, "Create");
	lua_pushlightuserdata(luaState, newRoom);
	if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	lua_pop(luaState, 1);

	//finish the routine
*/	return counter++;
}

int RoomManager::Load() {
	//TODO: RoomManageR::Load()
}

int RoomManager::Save(int uid) {
	//TODO: RoomManageR::Save(uid)
}

void RoomManager::Unload(int uid) {
/*	//find the room
	RoomData* room = elementMap.find(uid);
	if (room == elementMap.end()) {
		return;
	}

	//API hook
	lua_getglobal(luaState, TORTUGA_ROOM_NAME);
	lua_getfield(luaState, -1, "Unload");
	lua_pushlightuserdata(luaState, room);
	if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	lua_pop(luaState, 1);

	//free the memory
	delete room;
	roomMap.erase(uid);
*/}

void RoomManager::Delete(int uid) {
	//
}

void RoomManager::UnloadAll() {
/*	lua_getglobal(luaState, TORTUGA_ROOM_NAME);

	for (auto& it : roomMap) {
		//API hook
		lua_getfield(luaState, -1, "Unload");
		lua_pushlightuserdata(luaState, it.second);
		if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
		}
	}

	lua_pop(luaState, 1);
	roomMap.clear();
*/}

void RoomManager::UnloadIf(std::function<bool(std::pair<const int,RoomData>)> fn) {
	std::map<int, RoomData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

RoomData* RoomManager::Get(int uid) {
	return &elementMap[uid];
}

int RoomManager::GetLoadedCount() {
	return elementMap.size();
}

int RoomManager::GetTotalCount() {
	return elementMap.size();
}

std::map<int, RoomData>* RoomManager::GetContainer() {
	return &elementMap;
}
