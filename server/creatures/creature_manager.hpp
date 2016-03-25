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
#pragma once

#include "creature_data.hpp"

#include "lua.hpp"
#include "sqlite3.h"

#include <functional>
#include <map>
#include <string>

class CreatureManager {
public:
	CreatureManager();
	~CreatureManager();

	//common public methods
	void Update();

	int Create(std::string avatar, int scriptRef);
	void Unload(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, CreatureData const&>)> fn);

	//accessors & mutators
	CreatureData* Get(int uid);
	int GetLoadedCount();
	std::map<int, CreatureData>* GetContainer();

	//hooks
	lua_State* SetLuaState(lua_State* L);
	lua_State* GetLuaState();
	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();

private:
	//members
	std::map<int, CreatureData> elementMap;
	int counter = 0;
	lua_State* lua = nullptr;
	sqlite3* database = nullptr;
};
