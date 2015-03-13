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
#include "character_api.hpp"

#include "character_data.hpp"
#include "character_manager.hpp"
#include "entity_api.hpp"
#include "room_manager.hpp"
#include "server_utilities.hpp"

#include <stdexcept>

static int setRoomIndex(lua_State* L) { //TODO: (0) take the room userdata as a parameter
	//NOTE: type-dependant calls to various API functions, see bug #43

	//reverse engineer the character index
	int characterIndex = -1;
	CharacterData* character = static_cast<CharacterData*>(lua_touserdata(L, 1));
	int roomIndex = lua_tointeger(L, 2);
	RoomData* roomData = RoomManager::GetSingleton().Get(roomIndex);

	CharacterManager& characterMgr = CharacterManager::GetSingleton();

	for (auto& it : *characterMgr.GetContainer()) {
		if (character == &it.second) {
			characterIndex = it.first;
			break;
		}
	}

	//error checking
	if (characterIndex == -1) {
		throw(std::runtime_error("Lua Error: Failed to find character index by reference"));
	}

	//send the delete & create messages
	pumpAndChangeRooms(character, lua_tointeger(L, 2), characterIndex);
	return 0;
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
	{"SetRoomIndex", setRoomIndex},
//	{"GetOwner", getOwner}, //unusable without account API
	{"GetHandle", getHandle},
	{"GetAvatar", getAvatar},
	{nullptr, nullptr}
};

LUAMOD_API int openCharacterAPI(lua_State* L) {
	//get the parent table
	luaL_requiref(L, TORTUGA_ENTITY_API, openEntityAPI, false);

	//the local table
	luaL_newlib(L, characterLib);

	//merge the local table into the parent table
	lua_pushnil(L);	//first key
	while(lua_next(L, -2)) {
		//copy the key-value pair
		lua_pushvalue(L, -2);
		lua_pushvalue(L, -2);

		//push the copy to the parent table
		lua_settable(L, -6);

		//pop the original value before continuing
		lua_pop(L, 1);
	}

	//remove the local table, leaving the expanded parent table
	lua_pop(L, 1);

	return 1;
}