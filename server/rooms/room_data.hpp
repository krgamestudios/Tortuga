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
#pragma once

#include "barrier_manager.hpp"
#include "character_data.hpp"
#include "combat_instance_manager.hpp"
#include "creature_manager.hpp"
#include "region_pager_lua.hpp"
#include "trigger_manager.hpp"

#include "lua.hpp"
#include "sqlite3.h"

#include <list>
#include <string>

//DOCS: rooms are designed to collate all parts of the game that should be able to interact with each other

class RoomData {
public:
	RoomData() = default;
	~RoomData() = default;

	void RunFrame();

	//accessors and mutators
	std::string SetName(std::string);
	std::string GetName();

	std::string SetTileset(std::string);
	std::string GetTileset();

	int SetRoomIndex(int i);
	int GetRoomIndex();

	BarrierManager* GetBarrierMgr();
	std::list<CharacterData*>* GetCharacterList();
	CreatureManager* GetCreatureMgr();
	RegionPagerLua* GetPager();
	TriggerManager* GetTriggerMgr();

	//API interfaces
	lua_State* SetLuaState(lua_State* L);
	lua_State* GetLuaState();
	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();

	//hooks
	int SetTickReference(int i);
	int GetTickReference();
	//TODO: other triggers like player entry & exit, etc.

private:
	//metadata
	std::string roomName;
	std::string tilesetName;

	//members
	int roomIndex = 0;
	BarrierManager barrierMgr;
	std::list<CharacterData*> characterList;
	CombatInstanceManager combatInstanceMgr;
	CreatureManager creatureMgr;
	RegionPagerLua pager;
	TriggerManager triggerMgr;

	//API
	lua_State* lua = nullptr;
	sqlite3* database = nullptr;

	//hooks
	int tickRef = LUA_NOREF;
};
