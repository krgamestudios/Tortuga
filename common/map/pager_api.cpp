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
#include "pager_api.hpp"

#include "region_pager.hpp"
#include "region.hpp"

#include <stdexcept>
#include <string>

static int setTile(lua_State* L) {
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	int ret = pager->SetTile(lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4), lua_tointeger(L, 5));
	lua_pushinteger(L, ret);
	return 1;
}

static int getTile(lua_State* L) {
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	int ret = pager->GetTile(lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4));
	lua_pushinteger(L, ret);
	return 1;
}

static int getRegion(lua_State* L) {
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	Region* region = pager->GetRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	lua_pushlightuserdata(L, region);
	return 1;
}

static int setDirectory(lua_State* L) {
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	std::string s = pager->SetDirectory(lua_tostring(L, 2));
	lua_pushstring(L, s.c_str());
	return 1;
}

static int getDirectory(lua_State* L) {
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	std::string s = pager->GetDirectory();
	lua_pushstring(L, s.c_str());
	return 1;
}

static int loadRegion(lua_State* L) {
	//get the parameters
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	Region* region = pager->GetRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	std::string s = pager->GetDirectory();

	//push the parameters
	lua_getglobal(L, "region");
	lua_getfield(L, -1, "load");
	lua_pushlightuserdata(L, region);
	lua_pushstring(L, s.c_str());

	//call the method
	if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(L, -1) ));
	}
	return 1;
}

static int saveRegion(lua_State* L) {
	//get the parameters
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	Region* region = pager->GetRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	std::string s = pager->GetDirectory();

	//push the parameters
	lua_getglobal(L, "region");
	lua_getfield(L, -1, "save");
	lua_pushlightuserdata(L, region);
	lua_pushstring(L, s.c_str());

	//call the method
	if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(L, -1) ));
	}
	return 0;
}

static int createRegion(lua_State* L) {
	//get the parameters
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	Region* region = pager->GetRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));

	//push the parameters
	lua_getglobal(L, "region");
	lua_getfield(L, -1, "create");
	lua_pushlightuserdata(L, region);
	//TODO: parameters

	//call the method
	if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(L, -1) ));
	}
	return 0;
}

static int unloadRegion(lua_State* L) {
	//get the parameters
	RegionPager* pager = reinterpret_cast<RegionPager*>(lua_touserdata(L, 1));
	Region* region = pager->GetRegion(lua_tointeger(L, 2), lua_tointeger(L, 3));
	std::string s = pager->GetDirectory();

	//push the parameters
	lua_getglobal(L, "region");
	lua_getfield(L, -1, "unload");
	lua_pushlightuserdata(L, region);
	lua_pushstring(L, s.c_str());

	//call the method
	if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(L, -1) ));
	}
	return 0;
}

static const luaL_Reg pagerlib[] = {
	{"settile", setTile},
	{"gettile", getTile},
	{"getregion", getRegion},
	{"setdirectory", setDirectory},
	{"getdirectory", getDirectory},
	{"loadregion", loadRegion},
	{"saveregion", saveRegion},
	{"createregion", createRegion},
	{"unloadregion", unloadRegion},
	{nullptr, nullptr}
};

LUAMOD_API int luaopen_pagerapi(lua_State* L) {
	luaL_newlib(L, pagerlib);
	return 1;
}