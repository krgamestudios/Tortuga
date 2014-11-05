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

int createRoom(lua_State* L) {
	//create & get the room
	RoomManager& roomMgr = RoomManager::GetSingleton();
	int uid = roomMgr.Create("",""); //TODO: All new managers need their internals fleshed out
	RoomData* room = roomMgr.Get(uid);

	//setup the room
	//TODO: room parameters only set via lua, fix this
	room->SetRoomName(lua_tostring(L, 1));
	room->SetTilesetName(lua_tostring(L, 2));

	//return room, uid
	lua_pushlightuserdata(L, static_cast<void*>(room));
	lua_pushinteger(L, uid);

	return 2;
}

int unloadRoom(lua_State* L) {
	//TODO: check authorization for room deletion
	RoomManager& roomMgr = RoomManager::GetSingleton();
	roomMgr.Unload(lua_tointeger(L, 1));
	return 0;
}

static const luaL_Reg roomManagerLib[] = {
	{"CreateRoom", createRoom},
	{"UnloadRoom", unloadRoom},
	{nullptr, nullptr}
};

LUAMOD_API int openRoomManagerAPI(lua_State* L) {
	luaL_newlib(L, roomManagerLib);
	return 1;
}