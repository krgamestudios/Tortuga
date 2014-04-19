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
#ifndef MAPFILEFORMAT_HPP_
#define MAPFILEFORMAT_HPP_

#include "region.hpp"

#include "lua/lua.hpp"

#include <string>

class DummyFormat {
public:
	void Load(Region** const, int x, int y);
	void Save(Region* const);

	std::string SetSaveDir(std::string s) { return saveDir = s; }
	std::string GetSaveDir() { return saveDir; }
private:
	std::string saveDir;
};

//TODO: verbose save file format
//TODO: compact save file format

class LuaFormat {
public:
	void Load(Region** const, int x, int y);
	void Save(Region* const);

	std::string SetSaveDir(std::string s) { return saveDir = s; }
	std::string GetSaveDir() { return saveDir; }

	lua_State* SetLuaState(lua_State* L) { return state = L; }
	lua_State* GetLuaState() { return state; }
private:
	std::string saveDir;
	lua_State* state = nullptr;
};

#endif
