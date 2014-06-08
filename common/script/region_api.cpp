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
#include "region_api.hpp"

#include "region.hpp"

static int setTile(lua_State* L) {
	//operating on a region
	Region* ptr = (Region*)lua_touserdata(L, 1);
	ptr->SetTile(lua_tointeger(L, 2)-1, lua_tointeger(L, 3)-1, lua_tointeger(L, 4)-1, lua_tointeger(L, 5));
	return 0;
}

static int getTile(lua_State* L) {
	//operating on a region
	Region* ptr = (Region*)lua_touserdata(L, 1);
	int ret = ptr->GetTile(lua_tointeger(L, 2)-1, lua_tointeger(L, 3)-1, lua_tointeger(L, 4)-1);
	lua_pushnumber(L, ret);
	return 1;
}

static int getX(lua_State* L) {
	Region* ptr = (Region*)lua_touserdata(L, 1);
	lua_pushinteger(L, ptr->GetX());
	return 1;
}

static int getY(lua_State* L) {
	Region* ptr = (Region*)lua_touserdata(L, 1);
	lua_pushinteger(L, ptr->GetY());
	return 1;
}

static int getRegionWidth(lua_State* L) {
	lua_pushinteger(L, REGION_WIDTH);
	return 1;
}

static int getRegionHeight(lua_State* L) {
	lua_pushinteger(L, REGION_HEIGHT);
	return 1;
}

static int getRegionDepth(lua_State* L) {
	lua_pushinteger(L, REGION_DEPTH);
	return 1;
}

static const luaL_Reg regionlib[] = {
	{"settile",setTile},
	{"gettile",getTile},
	{"getx",getX},
	{"gety",getY},
	{"getregionwidth",getRegionWidth},
	{"getregionheight",getRegionHeight},
	{"getregiondepth",getRegionDepth},
	{nullptr, nullptr}
};

LUAMOD_API int luaopen_regionapi(lua_State* L) {
	luaL_newlib(L, regionlib);
	return 1;
}