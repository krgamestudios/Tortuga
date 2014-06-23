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
#include "region_pager_api.hpp"

#include "region_pager_lua.hpp"
#include "region.hpp"

#include <stdexcept>
#include <string>

//DOCS: These functions are just wrappers for the RegionPagerLua class

static int setTile(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	int ret = pager->SetTile(lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4), lua_tointeger(L, 5));
	lua_pushinteger(L, ret);
	return 1;
}

static int getTile(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	int ret = pager->GetTile(lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4));
	lua_pushinteger(L, ret);
	return 1;
}

static int getRegion(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	Region* region = pager->GetRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	lua_pushlightuserdata(L, region);
	return 1;
}

static int setDirectory(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	std::string s = pager->SetDirectory(lua_tostring(L, 2));
	lua_pushstring(L, s.c_str());
	return 1;
}

static int getDirectory(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	std::string s = pager->GetDirectory();
	lua_pushstring(L, s.c_str());
	return 1;
}

static int loadRegion(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	Region* region = pager->LoadRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	lua_pushlightuserdata(L, region);
	return 1;
}

static int saveRegion(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	Region* region = pager->SaveRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	lua_pushlightuserdata(L, region);
	return 1;
}

static int createRegion(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	Region* region = pager->CreateRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	lua_pushlightuserdata(L, region);
	return 1;
}

static int unloadRegion(lua_State* L) {
	RegionPagerLua* pager = reinterpret_cast<RegionPagerLua*>(lua_touserdata(L, 1));
	pager->UnloadRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	return 0;
}

static const luaL_Reg pagerlib[] = {
	{"SetTile", setTile},
	{"GetTile", getTile},
	{"GetRegion", getRegion},
	{"SetDirectory", setDirectory},
	{"GetDirectory", getDirectory},
	{"LoadRegion", loadRegion},
	{"SaveRegion", saveRegion},
	{"CreateRegion", createRegion},
	{"UnloadRegion", unloadRegion},
	{nullptr, nullptr}
};

LUAMOD_API int openRegionPagerAPI(lua_State* L) {
	luaL_newlib(L, pagerlib);
	return 1;
}