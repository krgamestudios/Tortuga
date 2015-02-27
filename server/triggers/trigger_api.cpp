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
#include "trigger_api.hpp"

#include "trigger_data.hpp"

//hamdle
static int setHandle(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	trigger->SetHandle(lua_tostring(L, 2));
	return 0;
}

static int getHandle(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	lua_pushstring(L, trigger->GetHandle().c_str());
	return 1;
}

//origin
static int setOrigin(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	trigger->SetOrigin(Vector2(lua_tonumber(L, 2), lua_tonumber(L, 3)));
	return 0;
}

static int getOrigin(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	lua_pushnumber(L, trigger->GetOrigin().x);
	lua_pushnumber(L, trigger->GetOrigin().y);
	return 2;
}

//bounds
static int setBoundingBox(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	trigger->SetBoundingBox(BoundingBox(
		lua_tonumber(L, 2),
		lua_tonumber(L, 3),
		lua_tonumber(L, 4),
		lua_tonumber(L, 5)
	));
	return 0;
}

static int getBoundingBox(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	lua_pushnumber(L, trigger->GetBoundingBox().x);
	lua_pushnumber(L, trigger->GetBoundingBox().y);
	lua_pushnumber(L, trigger->GetBoundingBox().w);
	lua_pushnumber(L, trigger->GetBoundingBox().h);
	return 4;
}

//triggers
static int setReference(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	luaL_unref(L, LUA_REGISTRYINDEX, trigger->GetScriptReference());
	trigger->SetScriptReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static int getReference(lua_State* L) {
	TriggerData* trigger = static_cast<TriggerData*>(lua_touserdata(L, 1));
	lua_pushinteger(L, trigger->GetScriptReference());
	lua_gettable(L, LUA_REGISTRYINDEX);
	return 1;
}

static const luaL_Reg triggerLib[] = {
	{"SetHandle", setHandle},
	{"GetHandle", getHandle},

	{"SetOrigin",setOrigin},
	{"GetOrigin",getOrigin},

	{"SetBounds",setBoundingBox},
	{"GetBounds",getBoundingBox},

	{"SetScript",setReference},
	{"GetScript",getReference},

	{nullptr, nullptr}
};

LUAMOD_API int openTriggerAPI(lua_State* L) {
	luaL_newlib(L, triggerLib);
	return 1;
}