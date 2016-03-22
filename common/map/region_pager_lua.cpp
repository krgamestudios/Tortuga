/* Copyright: (c) Kayne Ruse 2013-2016
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

RegionPagerLua::~RegionPagerLua() {
	//unload all regions
	UnloadAll();
	//clear any stored functions
	luaL_unref(lua, LUA_REGISTRYINDEX, loadRef);
	luaL_unref(lua, LUA_REGISTRYINDEX, saveRef);
	luaL_unref(lua, LUA_REGISTRYINDEX, createRef);
	luaL_unref(lua, LUA_REGISTRYINDEX, unloadRef);
}

//return the loaded region, or nullptr on failure
Region* RegionPagerLua::LoadRegion(int x, int y) {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, loadRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		//signal that there is no load function
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
	if (lua_isboolean(lua, -1) && lua_toboolean(lua, -1)) {
		lua_pop(lua, 1);
		//push and return the loaded region
		regionList.push_front(tmpRegion);
		return &regionList.front();
	}
	else {
		lua_pop(lua, 1);
		//signal a failure
		return nullptr;
	}
}

//NOTE: this return value seems strange; could replace it with a boolean
//return the saved region, or nullptr on failure
Region* RegionPagerLua::SaveRegion(int x, int y) {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, saveRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		//signal that the region wasn't saved
		return nullptr;
	}

	//find the specified region
	Region* ptr = FindRegion(x, y);
	if (!ptr) {
		lua_pop(lua, 1);
		//signal that there is no save function
		return nullptr;
	}
	lua_pushlightuserdata(lua, ptr);

	//call the function, 1 arg, 1 return
	if (lua_pcall(lua, 1, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
	}

	//check the return value, success or failure
	if (lua_isboolean(lua, -1) && lua_toboolean(lua, -1)) {
		lua_pop(lua, 1);
		//return the specified region that was saved
		return ptr;
	}
	else {
		lua_pop(lua, 1);
		//signal a failure
		return nullptr;
	}
}

//DOCS: since this method is the last ditch call from GetRegion, it must return a valid region object, even if the create function hasn't been set.
//return a new region, throwing an error on failure
Region* RegionPagerLua::CreateRegion(int x, int y) {
	if (FindRegion(x, y)) {
		throw(std::logic_error("Cannot overwrite an existing region"));
	}

	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, createRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		//return an empty region object
		regionList.emplace_front(x, y);
		return &regionList.front();
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

//no return
void RegionPagerLua::UnloadIf(std::function<bool(Region const&)> fn) {
	//get the pager's function from the registry
	lua_rawgeti(lua, LUA_REGISTRYINDEX, unloadRef);

	//check if this function is available
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		//remove the regions anyway
		regionList.remove_if(fn);
		return;
	}

	//run each region through this lambda
	regionList.remove_if([&](Region& region) -> bool {
		if (fn(region)) {

			//push a copy of the function onto the stack with the region
			lua_pushvalue(lua, -1);
			lua_pushlightuserdata(lua, static_cast<void*>(&region));

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
		//remove the regions anyway
		regionList.clear();
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