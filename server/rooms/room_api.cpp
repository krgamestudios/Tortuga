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

#include "room_manager.hpp"
#include "room_data.hpp"

static int getType(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushinteger(L, static_cast<int>(room->type));
	return 1;
}

//TODO: parameters

static int getRegionPager(lua_State* L) {
	RoomData* room = reinterpret_cast<RoomData*>(lua_touserdata(L, 1));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(&room->pager));
	return 1;
}

//TODO: generators

static const luaL_Reg roomlib[] = {
	{"gettype",getType},
	{"getregionpager",getRegionPager},
	{nullptr, nullptr}
};

LUAMOD_API int luaopen_roomapi(lua_State* L) {
	luaL_newlib(L, roomlib);
	return 1;
}