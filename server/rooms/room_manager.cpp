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

//-------------------------
//public access methods
//-------------------------

int RoomManager::Create(std::string roomName, std::string tileset) {
	//create the room
	RoomData* newRoom = &elementMap[counter]; //implicitly constructs the element
	newRoom->SetName(roomName);
	newRoom->SetTileset(tileset);

	newRoom->SetLuaState(lua);
	newRoom->SetDatabase(database);

	//get the hook
	lua_rawgeti(lua, LUA_REGISTRYINDEX, createRef);

	if(!lua_isnil(lua, -1)) {
		lua_pushlightuserdata(lua, newRoom);
		lua_pushinteger(lua, counter);
		//call the function, 2 arg, 0 return
		if (lua_pcall(lua, 2, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
		}
	}

	//finish the routine
	return counter++;
}

void RoomManager::UnloadAll() {
	//get the hook
	lua_rawgeti(lua, LUA_REGISTRYINDEX, unloadRef);

	if(!lua_isnil(lua, -1)) {
		//pass each room to the hook
		for (auto& it : elementMap) {
			lua_pushvalue(lua, -1); //copy the hook
			lua_pushlightuserdata(lua, &it.second);
			lua_pushinteger(lua, it.first);
			//call the function, 2 arg, 0 return
			if (lua_pcall(lua, 2, 0, 0) != LUA_OK) {
				throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
			}
		}
	}

	//pop the hook or nil
	lua_pop(lua, 1);

	elementMap.clear();
}

void RoomManager::UnloadIf(std::function<bool(std::pair<const int, RoomData const&>)> fn) {
	//get the hook
	lua_rawgeti(lua, LUA_REGISTRYINDEX, unloadRef);

	//get the element
	std::map<int, RoomData>::iterator it = elementMap.begin();

	//jenky pattern
	while (it != elementMap.end()) {
		if (fn(*it)) {

			if(!lua_isnil(lua, -1)) {
				lua_pushvalue(lua, -1); //copy the hook
				lua_pushlightuserdata(lua, &it->second);
				lua_pushinteger(lua, it->first);

				//call the function, 2 arg, 0 return
				if (lua_pcall(lua, 2, 0, 0) != LUA_OK) {
					throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
				}
			}

			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}

	//pop the hook or nil
	lua_pop(lua, 1);
}

void RoomManager::PushCharacter(CharacterData* character) {
	if (!character) {
		throw(std::runtime_error("Failed to push a null character to a room"));
	}

	RoomData* room = Get(character->GetRoomIndex());

	if (!room) {
		throw(std::runtime_error("Failed to push an character to a non-existant room"));
	}

	room->GetCharacterList()->push_back(character);
}

void RoomManager::PopCharacter(CharacterData const* character) {
	//NOTE: to pop an character from a room, the character must first exist
	if (!character) {
		throw(std::runtime_error("Failed to pop a null character to a room"));
	}

	RoomData* room = Get(character->GetRoomIndex());

	if (!room) {
		throw(std::runtime_error("Failed to pop an character to a non-existant room"));
	}

	room->GetCharacterList()->remove_if([character](CharacterData* ptr) {
		return character == ptr;
	});
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
	lua = L;
	for (auto& it : elementMap) {
		it.second.SetLuaState(lua);
	}
	return lua;
}

lua_State* RoomManager::GetLuaState() {
	return lua;
}

sqlite3* RoomManager::SetDatabase(sqlite3* db) {
	database = db;
	for (auto& it : elementMap) {
		it.second.SetDatabase(database);
	}
	return database;
}

sqlite3* RoomManager::GetDatabase() {
	return database;
}

int RoomManager::SetCreateReference(int i) {
	return createRef = i;
}

int RoomManager::SetUnloadReference(int i) {
	return unloadRef = i;
}

int RoomManager::GetCreateReference() {
	return createRef;
}

int RoomManager::GetUnloadReference() {
	return unloadRef;
}
