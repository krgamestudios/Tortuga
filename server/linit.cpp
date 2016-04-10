/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "barrier_api.hpp"
#include "barrier_manager_api.hpp"
#include "character_api.hpp"
#include "character_manager_api.hpp"
#include "region_api.hpp"
#include "region_pager_api.hpp"
#include "creature_api.hpp"
#include "creature_manager_api.hpp"
#include "network_api.hpp"
#include "room_api.hpp"
#include "room_manager_api.hpp"
#include "trigger_api.hpp"
#include "trigger_manager_api.hpp"

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
	{TORTUGA_BARRIER_API, openBarrierAPI},
	{TORTUGA_BARRIER_MANAGER_API, openBarrierManagerAPI},
	{TORTUGA_CHARACTER_API, openCharacterAPI},
	{TORTUGA_CHARACTER_MANAGER_API, openCharacterManagerAPI},
	{TORTUGA_CREATURE_API, openCreatureAPI},
	{TORTUGA_CREATURE_MANAGER_API, openCreatureManagerAPI},
	{TORTUGA_NETWORK_API, openNetworkAPI},
	{TORTUGA_REGION_API, openRegionAPI},
	{TORTUGA_REGION_PAGER_API, openRegionPagerAPI},
	{TORTUGA_ROOM_API, openRoomAPI},
	{TORTUGA_ROOM_MANAGER_API, openRoomManagerAPI},
	{TORTUGA_TRIGGER_API, openTriggerAPI},
	{TORTUGA_TRIGGER_MANAGER_API, openTriggerManagerAPI},
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
