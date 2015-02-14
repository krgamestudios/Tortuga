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
#include "room_manager_api.hpp"

#include "room_manager.hpp"

int createRoom(lua_State* L) {
	//create & get the room
	RoomManager& roomMgr = RoomManager::GetSingleton();
	int uid = roomMgr.Create(lua_tostring(L, 1), lua_tostring(L, 2));
	RoomData* room = roomMgr.Get(uid);

	//TODO: initialization parameters here?

	//return room, uid
	lua_pushlightuserdata(L, static_cast<void*>(room));
	lua_pushinteger(L, uid); //for debugging, mostly

	return 2;
}

int unloadRoom(lua_State* L) {
	RoomManager& roomMgr = RoomManager::GetSingleton();

	switch(lua_type(L, 1)) {
		case LUA_TNUMBER: {
			//number
			int uid = lua_tointeger(L, 1);
			roomMgr.UnloadIf([uid](std::pair<int, RoomData> it){
				return it.first == uid;
			});
		}
		break;
		case LUA_TSTRING: {
			//name
			std::string name = lua_tostring(L, 1);
			roomMgr.UnloadIf([name](std::pair<int, RoomData> it){
				return it.second.GetName() == name;
			});
		}
		break;
		case LUA_TLIGHTUSERDATA: {
			//the room itself
			std::string name = static_cast<RoomData*>(lua_touserdata(L, 1))->GetName();
			roomMgr.UnloadIf([name](std::pair<int, RoomData> it){
				return it.second.GetName() == name;
			});
		}
		break;
	}
	return 0;
}

int getRoom(lua_State* L) {
	//integer vs name for getRoom()
	RoomManager& roomMgr = RoomManager::GetSingleton();
	RoomData* room = nullptr;

	switch(lua_type(L, 1)) {
		case LUA_TNUMBER:
			//number
			room = roomMgr.Get(lua_tointeger(L, 1));
		break;
		case LUA_TSTRING:
			//name
			room = roomMgr.Get(lua_tostring(L, 1));
		break;
	}

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
	luaL_newlib(L, roomManagerLib);
	return 1;
}
