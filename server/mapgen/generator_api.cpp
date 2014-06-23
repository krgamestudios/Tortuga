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
#include "generator_api.hpp"

#include "base_generator.hpp"

static int getMapType(lua_State* L) {
	BaseGenerator* ptr = reinterpret_cast<BaseGenerator*>(lua_touserdata(L, 1));
	switch(ptr->GetMapType()) {
		case MapType::NONE:
			lua_pushstring(L, "none");
		break;
		case MapType::OVERWORLD:
			lua_pushstring(L, "overworld");
		break;
		case MapType::RUINS:
			lua_pushstring(L, "ruins");
		break;
		case MapType::TOWERS:
			lua_pushstring(L, "towers");
		break;
		case MapType::FORESTS:
			lua_pushstring(L, "forests");
		break;
		case MapType::CAVES:
			lua_pushstring(L, "caves");
		break;
	}
	return 1;
}

static int getChunk(lua_State* L) {
	BaseGenerator* ptr = reinterpret_cast<BaseGenerator*>(lua_touserdata(L, 1));
	ChunkData* chunk = ptr->GetChunk(lua_tointeger(L, 2), lua_tointeger(L, 3));
	lua_pushlightuserdata(L, reinterpret_cast<void*>(chunk));
	return 1;
}

static int getMapWidth(lua_State* L) {
	lua_pushinteger(L, MAP_WIDTH);
	return 1;
}

static int getMapHeight(lua_State* L) {
	lua_pushinteger(L, MAP_HEIGHT);
	return 1;
}

static const luaL_Reg generatorLib[] = {
	{"GetMapType", getMapType},
	{"GetChunk", getChunk},
	{"GetMapWidth", getMapWidth},
	{"GetMapHeight", getMapHeight},
	{nullptr, nullptr}
};

LUAMOD_API int openGeneratorAPI(lua_State* L) {
	luaL_newlib(L, generatorLib);
	return 1;
}
