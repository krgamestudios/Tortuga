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
#include "map_allocator.hpp"

#include <stdexcept>

void BlankAllocator::Create(Region** const ptr, int x, int y) {
	(*ptr) = new Region(x, y);
}

void BlankAllocator::Unload(Region* const ptr) {
	delete ptr;
}

void LuaAllocator::Create(Region** const ptr, int x, int y) {
	//something to work on
	(*ptr) = new Region(x, y);

	//API hook
	lua_getglobal(state, "map");
	lua_getfield(state, -1, "create");
	lua_pushlightuserdata(state, *ptr);
	if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(state, -1) ));
	}
	lua_pop(state, 1);
}

void LuaAllocator::Unload(Region* const ptr) {
	//API hook
	lua_getglobal(state, "map");
	lua_getfield(state, -1, "unload");
	lua_pushlightuserdata(state, ptr);
	if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(state, -1) ));
	}
	lua_pop(state, 1);

	//clean up the memory
	delete ptr;
}
