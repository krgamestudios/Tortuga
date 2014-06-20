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
#ifndef OVERWORLDGENERATOR_HPP_
#define OVERWORLDGENERATOR_HPP_

#include "chunk_data.hpp"

#include "lua/lua.hpp"

#define OVERWORLD_GENERATOR_PSEUDOINDEX "OverworldGenerator"

constexpr int OVERWORLD_WIDTH = 256;
constexpr int OVERWORLD_HEIGHT = 256;

//TODO: SuperGenerator API
//TODO: SuperGenerator DOCS
class OverworldGenerator {
public:
	OverworldGenerator();
	~OverworldGenerator();

	//accessors and mutators
	ChunkData* GetChunk(int x, int y) { return &chunks[x][y]; }

	lua_State* SetLuaState(lua_State* L) { return luaState = L; }
	lua_State* GetLuaState() { return luaState; }

private:
	ChunkData chunks[OVERWORLD_WIDTH][OVERWORLD_HEIGHT];
	lua_State* luaState = nullptr;
};

#endif
