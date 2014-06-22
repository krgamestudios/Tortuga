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
#include "region_pager_lua.hpp"

#include "utility.hpp"

#include <stdexcept>

Region* RegionPagerLua::LoadRegion(int x, int y) {
	//load the region if possible

	//something to work on
	regionList.emplace_front(x, y);

	//API hook
	lua_getglobal(luaState, "Region");
	lua_getfield(luaState, -1, "OnLoad");
	lua_pushlightuserdata(luaState, &regionList.front());
	lua_pushstring(luaState, directory.c_str());
	if (lua_pcall(luaState, 2, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	//success or failure
	if (!lua_toboolean(luaState, -1)) {
		lua_pop(luaState, 2);
		regionList.pop_front();
		return nullptr;
	}
	lua_pop(luaState, 2);
	return &regionList.front();
}

Region* RegionPagerLua::SaveRegion(int x, int y) {
	//find & save the region
	Region* ptr = FindRegion(x, y);
	if (ptr) {
		//API hook
		lua_getglobal(luaState, "Region");
		lua_getfield(luaState, -1, "OnSave");
		lua_pushlightuserdata(luaState, ptr);
		lua_pushstring(luaState, directory.c_str());
		if (lua_pcall(luaState, 2, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
		}
		lua_pop(luaState, 1);
	}
	return ptr;
}

Region* RegionPagerLua::CreateRegion(int x, int y) {
	if (FindRegion(x, y)) {
		throw(std::logic_error("Cannot overwrite an existing region"));
	}

	//something to work on
	regionList.emplace_front(x, y);

	//API hook
	lua_getglobal(luaState, "Region");
	lua_getfield(luaState, -1, "OnCreate");
	lua_pushlightuserdata(luaState, &regionList.front());
	//TODO: parameters
	if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	lua_pop(luaState, 1);
	return &regionList.front();;
}

void RegionPagerLua::UnloadRegion(int x, int y) {
	lua_getglobal(luaState, "Region");

	regionList.remove_if([&](Region& region) -> bool {
		if (region.GetX() == x && region.GetY() == y) {

			//API hook
			lua_getfield(luaState, -1, "OnUnload");
			lua_pushlightuserdata(luaState, &region);
			lua_pushstring(luaState, directory.c_str());
			if (lua_pcall(luaState, 2, 0, 0) != LUA_OK) {
				throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
			}

			return true;
		}
		return false;
	});

	lua_pop(luaState, 1);
}

void RegionPagerLua::UnloadAll() {
	lua_getglobal(luaState, "Region");

	for (auto& it : regionList) {
		//API hook
		lua_getfield(luaState, -1, "OnUnload");
		lua_pushlightuserdata(luaState, &it);
		lua_pushstring(luaState, directory.c_str());
		if (lua_pcall(luaState, 2, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
		}
	}

	lua_pop(luaState, 1);
	regionList.clear();
}