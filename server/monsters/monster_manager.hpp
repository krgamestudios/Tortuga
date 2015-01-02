/* Copyright: (c) Kayne Ruse 2013-2015
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
#ifndef MONSTERMANAGER_HPP_
#define MONSTERMANAGER_HPP_

#include "monster_data.hpp"
#include "singleton.hpp"

#include "lua.hpp"
#include "sqlite3.h"

#include <functional>
#include <map>
#include <string>

class MonsterManager: public Singleton<MonsterManager> {
public:
	//common public methods
	int Create(std::string);
	int Load(std::string);
	int Save(int uid);
	void Unload(int uid);
	void Delete(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, MonsterData const&>)> fn);

	//accessors & mutators
	MonsterData* Get(int uid);
	int GetLoadedCount();
	int GetTotalCount();
	std::map<int, MonsterData>* GetContainer();

	//hooks
	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();
	lua_State* SetLuaState(lua_State* L);
	lua_State* GetLuaState();

private:
	friend Singleton<MonsterManager>;

	MonsterManager() = default;
	~MonsterManager() = default;

	//members
	std::map<int, MonsterData> elementMap;
	sqlite3* database = nullptr;
	lua_State* lua = nullptr;
};

#endif