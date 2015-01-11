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
#include "waypoint_api.hpp"

#include "waypoint_data.hpp"

//origin
static int setOrigin(lua_State* L) {
	WaypointData* waypoint = static_cast<WaypointData*>(lua_touserdata(L, 1));
	waypoint->SetOrigin(Vector2(lua_tonumber(L, 2), lua_tonumber(L, 3)));
	return 0;
}

static int getOrigin(lua_State* L) {
	WaypointData* waypoint = static_cast<WaypointData*>(lua_touserdata(L, 1));
	lua_pushnumber(L, waypoint->GetOrigin().x);
	lua_pushnumber(L, waypoint->GetOrigin().y);
	return 2;
}

//bounds
static int setBoundingBox(lua_State* L) {
	WaypointData* waypoint = static_cast<WaypointData*>(lua_touserdata(L, 1));
	waypoint->SetBoundingBox(BoundingBox(
		lua_tonumber(L, 2),
		lua_tonumber(L, 3),
		lua_tonumber(L, 4),
		lua_tonumber(L, 5)
	));
	return 0;
}

static int getBoundingBox(lua_State* L) {
	WaypointData* waypoint = static_cast<WaypointData*>(lua_touserdata(L, 1));
	lua_pushnumber(L, waypoint->GetBoundingBox().x);
	lua_pushnumber(L, waypoint->GetBoundingBox().y);
	lua_pushnumber(L, waypoint->GetBoundingBox().w);
	lua_pushnumber(L, waypoint->GetBoundingBox().h);
	return 4;
}

//triggers
static int setTriggerReference(lua_State* L) {
	WaypointData* waypoint = static_cast<WaypointData*>(lua_touserdata(L, 1));
	luaL_unref(L, LUA_REGISTRYINDEX, waypoint->GetTriggerReference());
	waypoint->SetTriggerReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static int getTriggerReference(lua_State* L) {
	WaypointData* waypoint = static_cast<WaypointData*>(lua_touserdata(L, 1));
	lua_pushinteger(L, waypoint->GetTriggerReference());
	lua_gettable(L, LUA_REGISTRYINDEX);
	return 1;
}

static const luaL_Reg waypointLib[] = {
	{"SetOrigin",setOrigin},
	{"GetOrigin",getOrigin},

	{"SetBounds",setBoundingBox},
	{"GetBounds",getBoundingBox},

	{"SetTrigger",setTriggerReference},
	{"GetTrigger",getTriggerReference},
	{nullptr, nullptr}
};

LUAMOD_API int openWaypointAPI(lua_State* L) {
	luaL_newlib(L, waypointLib);
	return 1;
}