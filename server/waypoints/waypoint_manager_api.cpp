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
#include "waypoint_manager_api.hpp"

#include "waypoint_manager.hpp"

//TODO: figure out a way to iterate through elements of managers from lua

static int create(lua_State* L) {
	WaypointManager* mgr = static_cast<WaypointManager*>(lua_touserdata(L, 1));
	//TODO: create
}

static int unload(lua_State* L) {
	WaypointManager* mgr = static_cast<WaypointManager*>(lua_touserdata(L, 1));
	//TODO: create
}

static int getWaypoint(lua_State* L) {
	WaypointManager* mgr = static_cast<WaypointManager*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, mgr->Get(lua_tointeger(L, 2)));
	return 1;
}

static int getLoadedCount(lua_State* L) {
	WaypointManager* mgr = static_cast<WaypointManager*>(lua_touserdata(L, 1));
	lua_pushinteger(L, mgr->GetLoadedCount());
	return 1;
}

static const luaL_Reg waypointManagerLib[] = {
	{"Create",create},
	{"Unload",unload},
	{"GetWaypoint",getWaypoint},
	{"GetCount",getLoadedCount},
	{nullptr, nullptr}
};

LUAMOD_API int openWaypointManagerAPI(lua_State* L) {
	luaL_newlib(L, waypointManagerLib);
	return 1;
}