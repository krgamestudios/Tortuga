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
	Region tmpRegion(x, y);

	//API hook
	lua_getglobal(lua, "Region");
	lua_getfield(lua, -1, "Load");
	lua_pushlightuserdata(lua, &tmpRegion);
	if (lua_pcall(lua, 1, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
	}
	//success or failure
	if (!lua_toboolean(lua, -1)) {
		lua_pop(lua, 2);
		return nullptr;
	}
	lua_pop(lua, 2);
	regionList.push_front(tmpRegion);
	return &regionList.front();
}

Region* RegionPagerLua::SaveRegion(int x, int y) {
	//find & save the region
	Region* ptr = FindRegion(x, y);
	if (ptr) {
		//API hook
		lua_getglobal(lua, "Region");
		lua_getfield(lua, -1, "Save");
		lua_pushlightuserdata(lua, ptr);
		if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
		}
		lua_pop(lua, 1);
	}
	return ptr;
}

Region* RegionPagerLua::CreateRegion(int x, int y) {
	if (FindRegion(x, y)) {
		throw(std::logic_error("Cannot overwrite an existing region"));
	}

	//something to work on
	Region tmpRegion(x, y);

	//API hook
	lua_getglobal(lua, "Region");
	lua_getfield(lua, -1, "Create");
	lua_pushlightuserdata(lua, &tmpRegion);
	//TODO: parameters
	if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
	}
	lua_pop(lua, 1);
	regionList.push_front(tmpRegion);
	return &regionList.front();
}

void RegionPagerLua::UnloadRegion(int x, int y) {
	lua_getglobal(lua, "Region");

	regionList.remove_if([&](Region& region) -> bool {
		if (region.GetX() == x && region.GetY() == y) {

			//API hook
			lua_getfield(lua, -1, "Unload");
			lua_pushlightuserdata(lua, &region);
			if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
				throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
			}

			return true;
		}
		return false;
	});

	lua_pop(lua, 1);
}

void RegionPagerLua::UnloadAll() {
	lua_getglobal(lua, "Region");

	for (auto& it : regionList) {
		//API hook
		lua_getfield(lua, -1, "Unload");
		lua_pushlightuserdata(lua, &it);
		if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
		}
	}

	lua_pop(lua, 1);
	regionList.clear();
}