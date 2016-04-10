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
#include "creature_api.hpp"

#include "creature_data.hpp"

#include "entity_api.hpp"

static int setAvatar(lua_State* L) {
	CreatureData* creature = static_cast<CreatureData*>(lua_touserdata(L, 1));
	creature->SetAvatar(lua_tostring(L, 2));
	//TODO: (1) send an update to the clients?
	return 0;
}

static int getAvatar(lua_State* L) {
	CreatureData* creature = static_cast<CreatureData*>(lua_touserdata(L, 1));
	lua_pushstring(L, creature->GetAvatar().c_str());
	return 1;
}

static int setScript(lua_State* L) {
	CreatureData* creature = static_cast<CreatureData*>(lua_touserdata(L, 1));
	luaL_unref(L, LUA_REGISTRYINDEX, creature->GetScriptReference());
	creature->SetScriptReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static int getScript(lua_State* L) {
	CreatureData* creature = static_cast<CreatureData*>(lua_touserdata(L, 1));
	lua_pushinteger(L, creature->GetScriptReference());
	lua_gettable(L, LUA_REGISTRYINDEX);
	return 1;
}

//TODO: autogen docs
static int setTag(lua_State* L) {
	CreatureData* creature = static_cast<CreatureData*>(lua_touserdata(L, 1));
	creature->SetTag(lua_tostring(L, 2), lua_tostring(L, 3));
	return 0;
}

static int getTag(lua_State* L) {
	CreatureData* creature = static_cast<CreatureData*>(lua_touserdata(L, 1));
	lua_pushstring(L, creature->GetTag(lua_tostring(L, 2)).c_str());
	return 1;
}

static const luaL_Reg creatureLib[] = {
	{"SetAvatar", setAvatar},
	{"GetAvatar", getAvatar},
	{"SetScript", setScript},
	{"GetScript", getScript},
	{"SetTag", setTag},
	{"GetTag", getTag},
	{nullptr, nullptr}
};

LUAMOD_API int openCreatureAPI(lua_State* L) {
	//the local table
	luaL_newlib(L, creatureLib);

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