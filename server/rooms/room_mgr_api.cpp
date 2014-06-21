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
#include "room_mgr_api.hpp"

#include "room_manager.hpp"
#include "room_data.hpp"

#include <string>

static int getRoom(lua_State* L) {
	//get the room manager
	lua_pushstring(L, ROOM_MANAGER_PSEUDOINDEX);
	lua_gettable(L, LUA_REGISTRYINDEX);
	RoomManager* roomMgr = reinterpret_cast<RoomManager*>(lua_touserdata(L, -1));

	//push the room and return it
	lua_pushlightuserdata(L, reinterpret_cast<void*>( roomMgr->GetRoom(lua_tointeger(L, -2)) ));
	return 1;
}

static int createRoom(lua_State* L) {
	//get the room manager
	lua_pushstring(L, ROOM_MANAGER_PSEUDOINDEX);
	lua_gettable(L, LUA_REGISTRYINDEX);
	RoomManager* roomMgr = reinterpret_cast<RoomManager*>(lua_touserdata(L, -1));

	//determine the specified room type
	MapType mapType = [L]() -> MapType {
		if (std::string("overworld") == lua_tostring(L, -2)) return MapType::OVERWORLD;
		if (std::string("ruins") == lua_tostring(L, -2)) return MapType::RUINS;
		if (std::string("towers") == lua_tostring(L, -2)) return MapType::TOWERS;
		if (std::string("forests") == lua_tostring(L, -2)) return MapType::FORESTS;
		if (std::string("caves") == lua_tostring(L, -2)) return MapType::CAVES;
		return MapType::NONE;
	}();

	//create the room
	int newIndex = roomMgr->CreateRoom(mapType);

	//return the index
	lua_pushinteger(L, newIndex);
	return 1;
}

static int unloadRoom(lua_State* L) {
	//get the room manager
	lua_pushstring(L, ROOM_MANAGER_PSEUDOINDEX);
	lua_gettable(L, LUA_REGISTRYINDEX);
	RoomManager* roomMgr = reinterpret_cast<RoomManager*>(lua_touserdata(L, -1));

	//unload the specified room
	roomMgr->UnloadRoom(lua_tointeger(L, -2));

	return 0;
}

static const luaL_Reg roommgrlib[] = {
	{"getroom",getRoom},
	{"createroom",createRoom},
	{"unloadroom",unloadRoom},
	{nullptr, nullptr}
};

LUAMOD_API int luaopen_roommgrapi(lua_State* L) {
	luaL_newlib(L, roommgrlib);
	return 1;
}