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
#ifndef ROOMMANAGER_HPP_
#define ROOMMANAGER_HPP_

#include "room_data.hpp"

#include "lua/lua.hpp"

#include <map>

#define ROOM_MANAGER_PSEUDOINDEX "RoomManager"

class RoomManager {
public:
	RoomManager() = default;
	~RoomManager() = default;

	//public access methods
	//TODO: Fill this out

	//accessors and mutators
	RoomData* GetRoom(int uid);
	std::map<int, RoomData>* GetContainer();

	lua_State* SetLuaState(lua_State*);
	lua_State* GetLuaState();

private:
	std::map<int, RoomData> roomMap;
	lua_State* luaState = nullptr;
};

#endif