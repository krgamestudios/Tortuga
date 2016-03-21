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
#include "monster_manager_api.hpp"

#include "monster_manager.hpp"

static int create(lua_State* L) {
	MonsterManager* mgr = static_cast<MonsterManager* const>(lua_touserdata(L, 1));
	int index = mgr->Create(lua_tostring(L, 2), lua_tointeger(L, 3));
	MonsterData* monster = mgr->Get(index);
	lua_pushlightuserdata(L, static_cast<void*>(monster));
	lua_pushinteger(L, index);
	return 2;
}

//TOOD: this needs to take the userdata as a parameter too
static int unload(lua_State* L) {
	MonsterManager* mgr = static_cast<MonsterManager* const>(lua_touserdata(L, 1));
	mgr->Unload(lua_tointeger(L, 2));
	return 0;
}

static int unloadAll(lua_State* L) {
	MonsterManager* mgr = static_cast<MonsterManager* const>(lua_touserdata(L, 1));
	mgr->UnloadAll();
	return 0;
}

static int unloadIf(lua_State* L) {
	MonsterManager* mgr = static_cast<MonsterManager* const>(lua_touserdata(L, 1));
	//TODO: unloadIf
	return 0;
}

static int get(lua_State* L) {
	MonsterManager* mgr = static_cast<MonsterManager* const>(lua_touserdata(L, 1));
	MonsterData* monster = mgr->Get(lua_tointeger(L, 2));
	lua_pushlightuserdata(L, static_cast<void*>(monster));
	return 1;
}

static int getLoadedCount(lua_State* L) {
	MonsterManager* mgr = static_cast<MonsterManager* const>(lua_touserdata(L, 1));
	lua_pushinteger(L, mgr->GetLoadedCount());
	return 1;
}

static const luaL_Reg monsterManagerLib[] = {
	{"Create", create},
	{"Unload", unload},
	{"UnloadAll", unloadAll},
//	{"UnloadIf", unloadIf},
	{"Get", get},
	{"GetLoadedCount", getLoadedCount},
	{nullptr, nullptr}
};

LUAMOD_API int openMonsterManagerAPI(lua_State* L) {
	luaL_newlib(L, monsterManagerLib);
	return 1;
}