/* $Id: linit.c,v 1.32, modified
 * Initialization of libraries for lua.c and other clients
 * See Copyright Notice in lua.h
 * 
 * If you embed Lua in your program and need to open the standard
 * libraries, call luaL_openlibs in your program. If you need a
 * different set of libraries, copy this file to your project and edit
 * it to suit your needs.
 * 
 * Modified for use in Tortuga, renamed to linit.cpp
 * Modifications are released under the zlib license:
 * 
 * Copyright: (c) Kayne Ruse 2013-2015
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
#define linit_c
#define LUA_LIB

#include "lua.hpp"

#include "entity_api.hpp"
#include "character_system_api.hpp"
#include "map_system_api.hpp"
#include "monster_system_api.hpp"
#include "room_system_api.hpp"
#include "waypoint_system_api.hpp"

//these libs are loaded by lua.c and are readily available to any Lua program
static const luaL_Reg loadedlibs[] = {
	//Standard libs
	{"_G", luaopen_base},
	{LUA_LOADLIBNAME, luaopen_package},
	{LUA_COLIBNAME, luaopen_coroutine},
	{LUA_TABLIBNAME, luaopen_table},
	{LUA_IOLIBNAME, luaopen_io},
	{LUA_OSLIBNAME, luaopen_os},
	{LUA_STRLIBNAME, luaopen_string},
	{LUA_BITLIBNAME, luaopen_bit32},
	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_DBLIBNAME, luaopen_debug},
	{NULL, NULL}
};

//these libs are preloaded and must be required before used
static const luaL_Reg preloadedlibs[] = {
	{TORTUGA_ENTITY_API, openEntityAPI}, //required by derived classes
	{TORTUGA_CHARACTER_SYSTEM_API, openCharacterSystemAPI},
	{TORTUGA_MAP_SYSTEM_API, openMapSystemAPI},
	{TORTUGA_MONSTER_SYSTEM_API, openMonsterSystemAPI},
	{TORTUGA_ROOM_SYSTEM_API, openRoomSystemAPI},
	{TORTUGA_WAYPOINT_SYSTEM_API, openWaypointSystemAPI},
	{NULL, NULL}
};

LUALIB_API void luaL_openlibs (lua_State *L) {
	const luaL_Reg *lib;
	//call open functions from 'loadedlibs' and set results to global table
	for (lib = loadedlibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1); //remove lib
	}
	//add open functions from 'preloadedlibs' into 'package.preload' table
	luaL_getsubtable(L, LUA_REGISTRYINDEX, "_PRELOAD");
	for (lib = preloadedlibs; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	lua_pop(L, 1); //remove _PRELOAD table
}
