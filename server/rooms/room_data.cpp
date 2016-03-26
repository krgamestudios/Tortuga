/* Copyright: (c) Kayne Ruse 2013-2016
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

#include <algorithm>
#include <iostream>
#include <stack>
#include <stdexcept>

//TODO: (9) character collisions should be preformed client-side
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

	//build a list of characters for use with the triggers
	std::stack<Entity*> entityStack;
	for (auto& it : characterList) {
		entityStack.push(it);
	}

	//Compare the triggers to the entities, using their real hitboxes
	//NOTE: this stack solution should prevent problems when modifying the various lists
	while(entityStack.size()) {
		//get the entity & hitbox
		Entity* entity = entityStack.top();
		BoundingBox entityBox = entity->GetBounds() + entity->GetOrigin();

		//get the trigger pair & hitbox
		for (auto& triggerPair : *triggerMgr.GetContainer()) {
			BoundingBox triggerBox = triggerPair.second.GetBoundingBox() + triggerPair.second.GetOrigin();

			//find all collisions
			if (entityBox.CheckOverlap(triggerBox)) {
				//skip members of the exclusion list
				if (std::any_of(triggerPair.second.GetExclusionList()->begin(), triggerPair.second.GetExclusionList()->end(), [entity](Entity* ptr) -> bool {
					return entity == ptr;
				})) {
					continue;
				}

				//run the trigger script
				lua_rawgeti(lua, LUA_REGISTRYINDEX, triggerPair.second.GetScriptReference());
				lua_pushlightuserdata(lua, entity);

				if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
					//error
					throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
				}

				//push to the exclusion list
				triggerPair.second.GetExclusionList()->push_back(entity);
			}
			else {
				//remove members of the exclusion list
				//NOTE: characters in different rooms won't be removed, but that shouldn't be a problem
				triggerPair.second.GetExclusionList()->remove_if([entity](Entity* ptr) -> bool {
					return entity == ptr;
				});
			}
		}

		//next
		entityStack.pop();
	}

	//a list of creatures that need to be updated client-side
	std::list<CreatureData*> creatureList;
	creatureMgr.Update(&creatureList);

	//TODO: (0) send the updates

	//TODO: creature/character collisions
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

std::list<CharacterData*>* RoomData::GetCharacterList() {
	return &characterList;
}

CreatureManager* RoomData::GetCreatureMgr() {
	return &creatureMgr;
}

RegionPagerLua* RoomData::GetPager() {
	return &pager;
}

TriggerManager* RoomData::GetTriggerMgr() {
	return &triggerMgr;
}

lua_State* RoomData::SetLuaState(lua_State* L) {
	lua = L;
	creatureMgr.SetLuaState(lua);
	pager.SetLuaState(lua);
	triggerMgr.SetLuaState(lua);
	return lua;
}

lua_State* RoomData::GetLuaState() {
	return lua;
}

sqlite3* RoomData::SetDatabase(sqlite3* db) {
	database = db;
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