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
#include "room_manager_api.hpp"

#include "room_manager.hpp"

#include <string>
/*
static int getRoom(lua_State* L) {
	//find, push and return the room
	RoomData* room = RoomManager::GetSingleton().GetRoom(lua_tointeger(L, -2));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(room));
	return 1;
}

static int createRoom(lua_State* L) {
	//TODO: check parameter count for the glue functions

	//create, find and return the room
	int uid = RoomManager::GetSingleton().CreateRoom();
	lua_pushlightuserdata(L, RoomManager::GetSingleton().FindRoom(uid));
	return 1;
}

static int unloadRoom(lua_State* L) {
	//unload the specified room
	RoomManager::GetSingleton().UnloadRoom(lua_tointeger(L, -2));
	return 0;
}
*/
static const luaL_Reg roomManagerLib[] = {
//	{"GetRoom",getRoom},
//	{"CreateRoom",createRoom},
//	{"UnloadRoom",unloadRoom},
	{nullptr, nullptr}
};

LUAMOD_API int openRoomManagerAPI(lua_State* L) {
	luaL_newlib(L, roomManagerLib);
	return 1;
}