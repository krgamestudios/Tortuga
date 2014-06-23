/* Copyright: (c) Kayne Ruse 2014
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
#include "room_api.hpp"

#include "room_data.hpp"

static int getPager(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(&room->pager));
	return 1;
}

static int getGenerator(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(room->generator));
	return 1;
}

static int onCreate(lua_State* L) {
	//TODO: onCreate()
	return 0;
}

static int onUnload(lua_State* L) {
	//TODO: onUnload()
	return 0;
}

//TODO: parameters

static const luaL_Reg roomLib[] = {
	{"GetPager",getPager},
	{"GetGenerator",getGenerator},
	{"OnCreate", onCreate},
	{"OnUnload", onUnload},
	{nullptr, nullptr}
};

LUAMOD_API int openRoomAPI(lua_State* L) {
	luaL_newlib(L, roomLib);
	return 1;
}