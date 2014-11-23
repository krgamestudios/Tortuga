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

#include "room_system_api.hpp"
#include "room_api.hpp"

#include "room_manager.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

int createRoom(lua_State* L) {
	std::cout << "DEBUG: createRoom-1" << std::endl;

	//create & get the room
	RoomManager& roomMgr = RoomManager::GetSingleton();
	std::cout << "DEBUG: createRoom-2" << std::endl;
	int uid = roomMgr.Create(lua_tostring(L, 1));
	std::cout << "DEBUG: createRoom-3" << std::endl;
	RoomData* room = roomMgr.Get(uid);

	std::cout << "DEBUG: createRoom-4" << std::endl;

	//setup the room
	if (lua_gettop(L) > 1) {
		//ensure there are the correct number of parameters on the stack, including nil values
		lua_settop(L, 6); //5 parameters, + RoomName

		//get Room::Initialize() onto the top of the stack
		luaL_requiref(L, TORTUGA_ROOM_SYSTEM_API, openRoomSystemAPI, false);

		lua_pushstring(L, TORTUGA_ROOM_API);
		lua_gettable(L, -2); //RoomSystem[TORTUGA_ROOM_API]

		lua_pushstring(L, "Initialize");
		lua_gettable(L, -3); //Room[Initialize]

		//push the room onto the stack
		lua_pushlightuserdata(L, static_cast<void*>(room));

		//push (copies of) the parameters onto the stack
		if (lua_type(L, 2) == LUA_TTABLE) {
			//Unroll the table of parameters
			for (int i = 1; i <= 5; i++) { //should be 5 members, including nil values
				lua_rawgeti(L, 2, i);
			}
		}
		else {
			//copy the parameters
			for (int i = 2; i <= 6; i++) { //there are 5 parameters, including nil values, due to the call to lua_settop above
				lua_pushvalue(L, i);
			}
		}

		//by this point, the stack should look be ready to call Room::Initialize() with 6 parameters, including the room
		int result = lua_pcall(L, 6, 0, 0);

		if (result != LUA_OK) {
			//something went wrong
			std::ostringstream msg;
			msg << "Failed to initialize a room's parameters in RoomManagerAPI::create(): " << lua_tostring(L, -1);
			msg << "; Room name: " << lua_tostring(L, 1);
			throw(std::runtime_error(msg.str()));
		}

		//wow, that was hard
	}

	//return room, uid
	lua_pushlightuserdata(L, static_cast<void*>(room));
	lua_pushinteger(L, uid);

	return 2;
}

int unloadRoom(lua_State* L) {
	RoomManager& roomMgr = RoomManager::GetSingleton();

	//NOTE: the pager calls the unload function itself
	roomMgr.Unload(lua_tointeger(L, 1));

	return 0;
}

int getRoom(lua_State* L) {
	RoomManager& roomMgr = RoomManager::GetSingleton();

	RoomData* room = roomMgr.Get(lua_tointeger(L, 1));

	if (room) {
		lua_pushlightuserdata(L, static_cast<void*>(room));
	}
	else {
		lua_pushnil(L);
	}

	return 1;
}

static const luaL_Reg roomManagerLib[] = {
	{"CreateRoom", createRoom},
	{"UnloadRoom", unloadRoom},
	{"GetRoom", getRoom},
	{nullptr, nullptr}
};

LUAMOD_API int openRoomManagerAPI(lua_State* L) {
	std::cout << "DEBUG: openRoomManagerAPI" << std::endl;

	luaL_newlib(L, roomManagerLib);
	return 1;
}
