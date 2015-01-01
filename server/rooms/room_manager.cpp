/* Copyright: (c) Kayne Ruse 2013-2015
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
#include <sstream>

//debug
#include <iostream>

//-------------------------
//public access methods
//-------------------------

int RoomManager::Create(std::string roomName, std::string tileset) {
	//create the room
	RoomData* newRoom = &elementMap[counter]; //implicitly constructs the element
	newRoom->SetName(roomName);
	newRoom->SetTileset(tileset);

	newRoom->pager.SetLuaState(lua);

	//finish the routine
	return counter++;
}

void RoomManager::PushEntity(Entity const* entity) {
	if (!entity) {
		throw(std::runtime_error("Failed to push null entity"));
	}

	std::map<int, RoomData>::iterator it = elementMap.find(entity->GetRoomIndex());

	if (it == elementMap.end()) {
		std::ostringstream msg;
//		msg << "Failed to push entity; Room index not found: " << entity->GetRoomIndex() << std::endl;
		throw(std::runtime_error(msg.str()));
	}

	it->second.entityList.push_back(const_cast<Entity*>(entity));

	std::cout << "\troom[" << it->first << "].entityList.size(): " << it->second.entityList.size() << std::endl;
	std::cout << "\tEntity: " << int(entity) << "," << int(it->second.entityList.front()) << std::endl;
}

void RoomManager::PopEntity(Entity const* entity) {
	if (!entity) {
		throw(std::runtime_error("Failed to pop null entity"));
	}

	std::map<int, RoomData>::iterator it = elementMap.find(entity->GetRoomIndex());

	if (it == elementMap.end()) {
		std::ostringstream msg;
		msg << "Failed to pop entity; Room index not found: " << entity->GetRoomIndex() << std::endl;
		throw(std::runtime_error(msg.str()));
	}

	it->second.entityList.remove_if([entity](Entity* ptr) -> bool {
		bool b = (entity == ptr);
		std::cout << "\tmatch(" << int(ptr) << "," << int(entity) << "): " << b << std::endl;
		return b;
	});

	std::cout << "\troom[" << it->first << "].entityList.size(): " << it->second.entityList.size() << std::endl;
}

void RoomManager::UnloadAll() {
	elementMap.clear();
}

void RoomManager::UnloadIf(std::function<bool(std::pair<const int, RoomData const&>)> fn) {
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

RoomData* RoomManager::Get(std::string name) {
	for (std::map<int, RoomData>::iterator it = elementMap.begin(); it != elementMap.end(); ++it) {
		if (it->second.GetName() == name) {
			return &it->second;
		}
	}
	return nullptr;
}

int RoomManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, RoomData>* RoomManager::GetContainer() {
	return &elementMap;
}

lua_State* RoomManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* RoomManager::GetLuaState() {
	return lua;
}
