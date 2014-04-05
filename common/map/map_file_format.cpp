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
#include "map_file_format.hpp"

#include <stdexcept>

void DummyFormat::Load(Region** const ptr, int width, int height, int depth, int x, int y) {
	//EMPTY
}

void DummyFormat::Save(Region* const ptr) {
	//EMPTY
}
/*
void VerboseFormat::Load(Region** const ptr, int x, int y) {
	//TODO
}

void VerboseFormat::Save(Region* const ptr) {
	//TODO
}

void CompactFormat::Load(Region** const ptr, int x, int y) {
	//TODO
}

void CompactFormat::Save(Region* const ptr) {
	//TODO
}
*/
void LuaFormat::Load(Region** const ptr, int width, int height, int depth, int x, int y) {
	//something to load into
	(*ptr) = new Region(width, height, depth, x, y);

	//API hook
	lua_getglobal(state, "Region");
	lua_getfield(state, -1, "Load");
	lua_pushlightuserdata(state, *ptr);
	lua_pushstring(state, saveDir.c_str());
	if (lua_pcall(state, 2, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(state, -1) ));
	}
	if (lua_toboolean(state, -1) == false) {
		delete (*ptr);
		(*ptr) = nullptr;
	}
	lua_pop(state, 2);
}

void LuaFormat::Save(Region* const ptr) {
	//API hook
	lua_getglobal(state, "Region");
	lua_getfield(state, -1, "Save");
	lua_pushlightuserdata(state, ptr);
	lua_pushstring(state, saveDir.c_str());
	if (lua_pcall(state, 2, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(state, -1) ));
	}
	lua_pop(state, 1);
}