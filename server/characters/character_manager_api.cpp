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
#include "character_manager_api.hpp"

#include "character_manager.hpp"

static int setOnCreate(lua_State* L) {
	//TODO: (9) setOnCreate()
}

static int setOnLoad(lua_State* L) {
	//TODO: (9) setOnLoad()
}

static int setOnSave(lua_State* L) {
	//TODO: (9) setOnSave()
}

static int setOnUnload(lua_State* L) {
	//TODO: (9) setOnUnload()
}

static int setOnDelete(lua_State* L) {
	//TODO: (9) setOnDelete()
}

static int getCharacter(lua_State* L) {
	//integer vs name
	CharacterManager& characterMgr = CharacterManager::GetSingleton();
	CharacterData* characterData = nullptr;

	switch(lua_type(L, 1)) {
		case LUA_TNUMBER:
			characterData = characterMgr.Get(lua_tointeger(L, 1));
		break;
		case LUA_TSTRING:
			//access characters via their handles
			characterData = characterMgr.Get(lua_tostring(L, 1));
		break;
	}

	if (characterData) {
		lua_pushlightuserdata(L, static_cast<void*>(characterData));
	}
	else {
		lua_pushnil(L);
	}

	return 1;
}

static int getLoadedCount(lua_State* L) {
	CharacterManager& characterMgr = CharacterManager::GetSingleton();
	lua_pushinteger(L, characterMgr.GetLoadedCount());
	return 1;
}

static int forEach(lua_State* L) {
	//TODO: (1) find a way to update the clients when a script alters a character's data
}

static const luaL_Reg characterManagerLib[] = {
//	{"SetOnCreate", setOnCreate},
//	{"SetOnLoad", setOnLoad},
//	{"SetOnSave", setOnSave},
//	{"SetOnUnload", setOnUnload},
//	{"SetOnDelete", setOnDelete},
	{"GetCharacter", getCharacter},
	{"GetLoadedCount", getLoadedCount},
//	{"ForEach", forEach},
	{nullptr, nullptr}
};

LUAMOD_API int openCharacterManagerAPI(lua_State* L) {
	luaL_newlib(L, characterManagerLib);
	return 1;
}