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
	Region* region = reinterpret_cast<Region*>(lua_touserdata(L, 1));
	int ret = region->SetTile(lua_tointeger(L, 2)-1, lua_tointeger(L, 3)-1, lua_tointeger(L, 4)-1, lua_tointeger(L, 5));
	lua_pushinteger(L, ret);
	return 1;
}

static int getTile(lua_State* L) {
	Region* region = reinterpret_cast<Region*>(lua_touserdata(L, 1));
	int ret = region->GetTile(lua_tointeger(L, 2)-1, lua_tointeger(L, 3)-1, lua_tointeger(L, 4)-1);
	lua_pushinteger(L, ret);
	return 1;
}

static int getX(lua_State* L) {
	Region* region = reinterpret_cast<Region*>(lua_touserdata(L, 1));
	lua_pushinteger(L, region->GetX());
	return 1;
}

static int getY(lua_State* L) {
	Region* region = reinterpret_cast<Region*>(lua_touserdata(L, 1));
	lua_pushinteger(L, region->GetY());
	return 1;
}

static int getWidth(lua_State* L) {
	lua_pushinteger(L, REGION_WIDTH);
	return 1;
}

static int getHeight(lua_State* L) {
	lua_pushinteger(L, REGION_HEIGHT);
	return 1;
}

static int getDepth(lua_State* L) {
	lua_pushinteger(L, REGION_DEPTH);
	return 1;
}

static int onLoad(lua_State* L) {
	//TODO: onLoad()
	lua_pushboolean(L, false);
	return 1;
}

static int onSave(lua_State* L) {
	//TODO: onSave()
	return 0;
}

static int onCreate(lua_State* L) {
	//TODO: onCreate()
	return 0;
}

static int onUnload(lua_State* L) {
	//TODO: onUnload()
	return 0;
}

static const luaL_Reg regionLib[] = {
	{"SetTile",setTile},
	{"GetTile",getTile},
	{"GetX",getX},
	{"GetY",getY},
	{"GetWidth",getWidth},
	{"GetHeight",getHeight},
	{"GetDepth",getDepth},
	{"OnLoad",onLoad},
	{"OnSave",onSave},
	{"OnCreate",onCreate},
	{"OnUnload",onUnload},
	{nullptr, nullptr}
};

LUAMOD_API int openRegionAPI(lua_State* L) {
	luaL_newlib(L, regionLib);
	return 1;
}