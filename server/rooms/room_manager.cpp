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
	//create the room
	RoomData* newRoom = &elementMap[counter]; //implicitly constructs the element
	newRoom->pager.SetLuaState(lua);

	//finish the routine
	return counter++;
}

int RoomManager::Load() {
	//TODO: RoomManager::Load()
	return -1;
}

int RoomManager::Save(int uid) {
	//TODO: RoomManager::Save(uid)
	return -1;
}

void RoomManager::Unload(int uid) {
	//find the room
	std::map<int, RoomData>::iterator it = elementMap.find(uid);
	if (it == elementMap.end()) {
		return;
	}

	//free the memory
	elementMap.erase(uid);
}

void RoomManager::Delete(int uid) {
	//TODO: RoomManager::Delete(int uid)
	//NOTE: aliased to RoomManager::Unload(int uid)
	Unload(uid);
}

void RoomManager::UnloadAll() {
	elementMap.clear();
}

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
	std::map<int, RoomData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
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
