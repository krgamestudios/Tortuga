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
#include "room_api.hpp"

#include "room_data.hpp"

#include <sstream>
#include <stdexcept>

static int setRoomName(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	room->SetName(lua_tostring(L, 2));
	return 0;
}

static int getRoomName(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushstring(L, room->GetName().c_str());
	return 1;
}

static int setTilesetName(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	room->SetTileset(lua_tostring(L, 2));
	return 0;
}

static int getTilesetName(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushstring(L, room->GetTileset().c_str());
	return 1;
}

static int getPager(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(room->GetPager()) );
	return 1;
}

static int getMonsterMgr(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(room->GetMonsterMgr()) );
	return 1;
}

static int getTriggerMgr(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(room->GetTriggerMgr()) );
	return 1;
}

static int forEachCharacter(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	//pass each character to the given function
	for (auto& it : *room->GetCharacterList()) {
		lua_pushvalue(L, -1);
		lua_pushlightuserdata(L, static_cast<void*>(it));
		//call each iteration, throwing an exception if something happened
		if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
			std::ostringstream os;
			os << "Lua error: ";
			os << lua_tostring(L, -1);
			throw(std::runtime_error(os.str()));
		}
	}
	return 0;
}

static int forEachMonster(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	MonsterManager* monsterMgr = room->GetMonsterMgr();
	//pass each monster to the given function
	for (auto& it : *monsterMgr->GetContainer()) {
		lua_pushvalue(L, -1);
		lua_pushlightuserdata(L, static_cast<void*>(&it.second));
		//call each iteration, throwing an exception if something happened
		if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
			std::ostringstream os;
			os << "Lua error: ";
			os << lua_tostring(L, -1);
			throw(std::runtime_error(os.str()));
		}
	}
	return 0;
}

static int setOnTick(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	luaL_unref(L, LUA_REGISTRYINDEX, room->GetTickReference());
	room->SetTickReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static int getOnTick(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_rawgeti(L, LUA_REGISTRYINDEX, room->GetTickReference());
	return 1;
}

static int initialize(lua_State* L) {
	RoomData* room = static_cast<RoomData*>(lua_touserdata(L, 1));

	//set the refs of these parameters (backwards, since it pops from the top of the stack)
	room->GetPager()->SetUnloadReference(luaL_ref(L, LUA_REGISTRYINDEX));
	room->GetPager()->SetCreateReference(luaL_ref(L, LUA_REGISTRYINDEX));
	room->GetPager()->SetSaveReference(luaL_ref(L, LUA_REGISTRYINDEX));
	room->GetPager()->SetLoadReference(luaL_ref(L, LUA_REGISTRYINDEX));

	//more parameters can be added here later
	return 0;
}

static const luaL_Reg roomLib[] = {
	{"SetName", setRoomName},
	{"GetName", getRoomName},
	{"SetTileset", setTilesetName},
	{"GetTileset", getTilesetName},

	{"GetPager",getPager},
	{"GetMonsterMgr",getMonsterMgr},
	{"GetTriggerMgr",getTriggerMgr},

	{"ForEachCharacter", forEachCharacter},
	{"ForEachMonster", forEachMonster},

	{"SetOnTick", setOnTick},
	{"GetOnTick", getOnTick},

	{"Initialize", initialize},
	{nullptr, nullptr}
};

LUAMOD_API int openRoomAPI(lua_State* L) {
	luaL_newlib(L, roomLib);
	return 1;
}