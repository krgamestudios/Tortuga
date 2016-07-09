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

#include "barrier_data.hpp"

#include "lua.hpp"
#include "sqlite3.h"

#include <algorithm>
#include <functional>
#include <list>
#include <map>

class BarrierManager {
public:
	BarrierManager();
	~BarrierManager();

	//common public methods
	void Update(std::list<std::pair<const int, BarrierData*>>* barrierList, bool updateAll);

	int Create(int instanceIndex);
	void Unload(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, BarrierData const&>)> fn);

	//accessors & mutators
	BarrierData* Find(int uid);
	int GetLoadedCount();
	std::map<int, BarrierData>* GetContainer();

	//hooks
	lua_State* SetLuaState(lua_State* L);
	lua_State* GetLuaState();
	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();

	int SetCreateReference(int i);
	int SetUnloadReference(int i);

	int GetCreateReference();
	int GetUnloadReference();

private:
	//members
	std::map<int, BarrierData> elementMap;
	int counter = 0;
	lua_State* lua = nullptr;
	sqlite3* database = nullptr;
	int createRef = LUA_NOREF;
	int unloadRef = LUA_NOREF;
};