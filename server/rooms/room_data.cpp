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
#include "room_data.hpp"

#include <iostream>
#include <stdexcept>

void RoomData::RunFrame() {
	//get the hook
	lua_rawgeti(lua, LUA_REGISTRYINDEX, tickRef);

	if (!lua_isnil(lua, -1)) {
		//call the tick function, with this as a parameter
		lua_pushlightuserdata(lua, this);
		if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
		}
	}
	else {
		lua_pop(lua, 1);
	}

	//update the entities in the room
	for (auto& it : characterList) {
		it->Update();
	}
	for (auto& it : *monsterMgr.GetContainer()) {
		it.second.Update();
	}

	//compare the triggers to the entities, using their real hitboxes
	for (auto& it : *triggerMgr.GetContainer()) {
		BoundingBox itBox = it.second.GetBoundingBox() + it.second.GetOrigin();
		for (auto& character : characterList) {
			BoundingBox hitBox = character->GetBounds() + character->GetOrigin();

			if ( itBox.CheckOverlap(hitBox) ) {
				//trigger script
				lua_rawgeti(lua, LUA_REGISTRYINDEX, it.second.GetScriptReference());
				lua_pushlightuserdata(lua, character); //TODO: (1) entity type

				//run the script
				if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
					//error
					throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
				}
			}
		}
//		for (auto& monster : *monsterMgr.GetContainer()) {
//			if (it.second.Compare(static_cast<Entity*>(&monster.second))) {
//				//TODO: (1) trigger script
//			}
//		}
	}
}

std::string RoomData::SetName(std::string s) {
	return roomName = s;
}

std::string RoomData::GetName() {
	return roomName;
}

std::string RoomData::SetTileset(std::string s) {
	return tilesetName = s;
}

std::string RoomData::GetTileset() {
	return tilesetName;
}

RegionPagerLua* RoomData::GetPager() {
	return &pager;
}

MonsterManager* RoomData::GetMonsterMgr() {
	return &monsterMgr;
}

TriggerManager* RoomData::GetTriggerMgr() {
	return &triggerMgr;
}

std::list<CharacterData*>* RoomData::GetCharacterList() {
	return &characterList;
}

lua_State* RoomData::SetLuaState(lua_State* L) {
	lua = L;
	pager.SetLuaState(lua);
	monsterMgr.SetLuaState(lua);
	triggerMgr.SetLuaState(lua);
	return lua;
}

lua_State* RoomData::GetLuaState() {
	return lua;
}

sqlite3* RoomData::SetDatabase(sqlite3* db) {
	database = db;
	monsterMgr.SetDatabase(database);
	return database;
}

sqlite3* RoomData::GetDatabase() {
	return database;
}

int RoomData::SetTickReference(int i) {
	return tickRef = i;
}

int RoomData::GetTickReference() {
	return tickRef;
}