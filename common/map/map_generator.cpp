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
#include "map_generator.hpp"

void BlankGenerator::Create(Region** const ptr, int width, int height, int depth, int x, int y) {
	(*ptr) = new Region(width, height, depth, x, y);
}

void BlankGenerator::Unload(Region* const ptr) {
	delete ptr;
}
/*
void PerlinGenerator::Create(Region** const ptr, int width, int height, int depth, int x, int y) {
	(*ptr) = new Region(width, height, depth, x, y);
}

void PerlinGenerator::Unload(Region* const ptr) {
	delete ptr;
}
*/
void LuaGenerator::Create(Region** const ptr, int width, int height, int depth, int x, int y) {
	(*ptr) = new Region(width, height, depth, x, y);

	//generate the lua-driven maps
	lua_getglobal(state, "CreateRegion");
	lua_pushlightuserdata(state, ptr);
	lua_pcall(state, 1, 0, 0);
}

void LuaGenerator::Unload(Region* const ptr) {
	delete ptr;
}
