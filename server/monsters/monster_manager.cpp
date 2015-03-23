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
#include "monster_manager.hpp"

MonsterManager::MonsterManager() {
	//EMPTY
}

MonsterManager::~MonsterManager() {
	UnloadAll();
}

int MonsterManager::Create(std::string s) {
	//TODO: (1) MonsterManager::Create()
}

void MonsterManager::Unload(int uid) {
	elementMap.erase(uid);
}

void MonsterManager::UnloadAll() {
	elementMap.clear();
}

void MonsterManager::UnloadIf(std::function<bool(std::pair<const int, MonsterData const&>)> fn) {
	std::map<int, MonsterData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

MonsterData* MonsterManager::Get(int uid) {
	std::map<int, MonsterData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int MonsterManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, MonsterData>* MonsterManager::GetContainer() {
	return &elementMap;
}

lua_State* MonsterManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* MonsterManager::GetLuaState() {
	return lua;
}

sqlite3* MonsterManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* MonsterManager::GetDatabase() {
	return database;
}
