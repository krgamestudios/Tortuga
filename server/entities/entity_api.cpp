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
#include "entity_api.hpp"

#include "entity.hpp"

static int setRoomIndex(lua_State* L) {
	Entity* entity = static_cast<Entity*>(lua_touserdata(L, 1));
	entity->SetRoomIndex(lua_tointeger(L, 2));
	return 0;
}

static int setOrigin(lua_State* L) {
	Entity* entity = static_cast<Entity*>(lua_touserdata(L, 1));
	entity->SetOrigin({lua_tonumber(L, 2), lua_tonumber(L, 3)});
	return 0;
}

static int setMotion(lua_State* L) {
	Entity* entity = static_cast<Entity*>(lua_touserdata(L, 1));
	entity->SetMotion({lua_tonumber(L, 2), lua_tonumber(L, 3)});
	return 0;
}

static int getRoomIndex(lua_State* L) {
	Entity* entity = static_cast<Entity*>(lua_touserdata(L, 1));
	lua_pushinteger(L, entity->GetRoomIndex());
	return 1;
}

static int getOrigin(lua_State* L) {
	Entity* entity = static_cast<Entity*>(lua_touserdata(L, 1));
	lua_pushnumber(L, entity->GetOrigin().x);
	lua_pushnumber(L, entity->GetOrigin().y);
	return 2;
}

static int getMotion(lua_State* L) {
	Entity* entity = static_cast<Entity*>(lua_touserdata(L, 1));
	lua_pushnumber(L, entity->GetOrigin().x);
	lua_pushnumber(L, entity->GetOrigin().y);
	return 2;
}

static const luaL_Reg entityLib[] = {
	{"SetRoomIndex", setRoomIndex},
	{"SetOrigin", setOrigin},
	{"SetMotion", setMotion},
	{"GetRoomIndex", getRoomIndex},
	{"GetOrigin", getOrigin},
	{"GetMotion", getMotion},
	{nullptr, nullptr}
};

LUAMOD_API int openEntityAPI(lua_State* L) {
	luaL_newlib(L, entityLib);
	return 1;
}