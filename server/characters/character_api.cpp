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
#include "character_api.hpp"

#include "character_data.hpp"
#include "character_manager.hpp"
#include "entity_api.hpp"
#include "room_manager.hpp"
#include "server_utilities.hpp"

#include <stdexcept>
#include <iostream>
static int setRoom(lua_State* L) {
	//variables
	CharacterData* character = static_cast<CharacterData*>(lua_touserdata(L, 1));
	CharacterManager& characterMgr = CharacterManager::GetSingleton();

	//error checking
	if (characterMgr.Find(character->GetIndex()) != character) {
		throw(std::runtime_error("Lua Error: Failed to verify character index by reference"));
	}

	//get the room index, depending on the parameter type
	int roomIndex = -1;
	switch(lua_type(L, 2)) {
		case LUA_TNUMBER:
			//simple integer
			roomIndex = lua_tointeger(L, 2);
		break;
		case LUA_TLIGHTUSERDATA: {
			//check that this is a room first
			RoomData* room = static_cast<RoomData*>(lua_touserdata(L, 2));
			RoomManager& roomMgr = RoomManager::GetSingleton();

			if (roomMgr.Find(room->GetRoomIndex()) != room) {
				std::cout << room->GetRoomIndex() << std::endl;
				throw(std::runtime_error("Lua Error: Failed to verify room index by reference"));
			}
			roomIndex = room->GetRoomIndex();
		}
		break;
		default:
			throw(std::runtime_error("Lua Error: Failed to find room index by reference"));
	}

	//send the delete & create messages
	pumpAndChangeRooms(character, roomIndex, character->GetIndex());
	return 0;
}

static int getIndex(lua_State* L) {
	CharacterData* character = static_cast<CharacterData*>(lua_touserdata(L, 1));
	lua_pushinteger(L, character->GetIndex());
	return 1;
}

static int getOwner(lua_State* L) {
	CharacterData* character = static_cast<CharacterData*>(lua_touserdata(L, 1));
	lua_pushinteger(L, character->GetOwner());
	return 1;
}

static int getHandle(lua_State* L) {
	CharacterData* character = static_cast<CharacterData*>(lua_touserdata(L, 1));
	lua_pushstring(L, character->GetHandle().c_str());
	return 1;
}

static int getAvatar(lua_State* L) {
	CharacterData* character = static_cast<CharacterData*>(lua_touserdata(L, 1));
	lua_pushstring(L, character->GetAvatar().c_str());
	return 1;
}

static const luaL_Reg characterLib[] = {
	{"SetRoom", setRoom},
	{"GetIndex", getIndex},
//	{"GetOwner", getOwner}, //unusable without account API
	{"GetHandle", getHandle},
	{"GetAvatar", getAvatar},
	{nullptr, nullptr}
};

LUAMOD_API int openCharacterAPI(lua_State* L) {
	//the local table
	luaL_newlib(L, characterLib);

	//get the parent table
	luaL_requiref(L, TORTUGA_ENTITY_API, openEntityAPI, false);

	//merge the parent table into the local table
	lua_pushnil(L);	//first key
	while(lua_next(L, -2)) {
		//copy the key-value pair
		lua_pushvalue(L, -2);
		lua_pushvalue(L, -2);

		//push the copy to the local table
		lua_settable(L, -6);

		//pop the original value before continuing
		lua_pop(L, 1);
	}

	//remove the parent table, leaving the expanded local table
	lua_pop(L, 1);

	return 1;
}