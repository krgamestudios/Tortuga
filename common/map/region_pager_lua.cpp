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

#include <stdexcept>

//TODO: Could I push the pager to the API functions too?

Region* RegionPagerLua::LoadRegion(int x, int y) {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, loadRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		return nullptr;
	}

	//something to work on
	Region tmpRegion(x, y);
	lua_pushlightuserdata(lua, &tmpRegion);

	//call the funtion, 1 arg, 1 return
	if (lua_pcall(lua, 1, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
	}

	//check the return value, success or failure
	if (lua_toboolean(lua, -1)) {
		lua_pop(lua, 1);
		regionList.push_front(tmpRegion);
		return &regionList.front();
	}
	else {
		lua_pop(lua, 1);
		return nullptr;
	}
}

Region* RegionPagerLua::SaveRegion(int x, int y) {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, saveRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		return nullptr;
	}

	//find the specified region
	Region* ptr = FindRegion(x, y);
	if (!ptr) {
		lua_pop(lua, 1);
		return nullptr;
	}
	lua_pushlightuserdata(lua, ptr);

	//call the function, 1 arg, 1 return
	if (lua_pcall(lua, 1, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
	}

	//check the return value, success or failure
	if (lua_toboolean(lua, -1)) {
		lua_pop(lua, 1);
		return ptr;
	}
	else {
		lua_pop(lua, 1);
		return nullptr;
	}
}

Region* RegionPagerLua::CreateRegion(int x, int y) {
	if (FindRegion(x, y)) {
		throw(std::logic_error("Cannot overwrite an existing region"));
	}

	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, createRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		return nullptr;
	}

	//something to work on
	Region tmpRegion(x, y);
	lua_pushlightuserdata(lua, &tmpRegion);

	//call the function, 1 arg, 0 return
	if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
	}

	//return the new region
	regionList.push_front(tmpRegion);
	return &regionList.front();
}

void RegionPagerLua::UnloadRegion(int x, int y) {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, unloadRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		return;
	}

	//run each region through this lambda
	regionList.remove_if([&](Region& region) -> bool {
		if (region.GetX() == x && region.GetY() == y) {

			//push a copy of the function onto the stack with the region
			lua_pushvalue(lua, -1);
			lua_pushlightuserdata(lua, &region);

			//call the function, 1 arg, 0 return
			if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
				throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
			}

			//signal to the container
			return true;
		}
		//signal to the container
		return false;
	});

	//pop the base copy of the function
	lua_pop(lua, 1);
}

void RegionPagerLua::UnloadAll() {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, unloadRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		return;
	}

	for (auto& it : regionList) {
		//push a copy of the function onto the stack with the region
		lua_pushvalue(lua, -1);
		lua_pushlightuserdata(lua, &it);

		//call the function, 1 arg, 0 return
		if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
		}
	}

	//pop the base copy of the function
	lua_pop(lua, 1);

	//remove from memory
	regionList.clear();
}