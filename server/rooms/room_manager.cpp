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

#include <stdexcept>

//-------------------------
//public access methods
//-------------------------

RoomData* RoomManager::CreateRoom(int uid) {
	//don't overwrite existing rooms
	std::map<int, RoomData*>::iterator it = roomMap.find(uid);
	if (it != roomMap.end()) {
		throw(std::runtime_error("Cannot overwrite an existing room"));
	}
	roomMap[uid] = new RoomData();
	//TODO: create room in the API
	if (luaState) {
		roomMap[uid]->pager.SetLuaState(luaState);
	}
	return roomMap[uid];
}

RoomData* RoomManager::UnloadRoom(int uid) {
	//TODO: unload room in the API
	delete roomMap[uid];
	roomMap.erase(uid);
}

RoomData* RoomManager::GetRoom(int uid) {
	RoomData* ptr = FindRoom(uid);
	if (ptr) return ptr;
	ptr = CreateRoom(uid);
	return ptr;
}

RoomData* RoomManager::FindRoom(int uid) {
	std::map<int, RoomData*>::iterator it = roomMap.find(uid);
	if (it == roomMap.end()) {
		return nullptr;
	}
	return it->second;
}

RoomData* RoomManager::PushRoom(int uid, RoomData* room) {
	//unload existing rooms with this index
	std::map<int, RoomData*>::iterator it = roomMap.find(uid);
	if (it != roomMap.end()) {
		UnloadRoom(uid);
	}
	roomMap[uid] = room;
}
