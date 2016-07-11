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
#include "battle_manager.hpp"

BattleManager::BattleManager() {
	//EMPTY
}

BattleManager::~BattleManager() {
	UnloadAll();
}

//arg: a list of combats to be updated in the clients
void BattleManager::Update() {
	for (auto& it : elementMap) {
		it.second.Update();
	}
}

int BattleManager::Create() {
	//implicitly create the new object
	elementMap.emplace( std::pair<int, Battle>(counter, Battle()) );

	//TODO: do various things like saving to the database
	return counter++;
}

//TODO: (1) combat load, save

void BattleManager::Unload(int uid) {
	elementMap.erase(uid);
}

void BattleManager::UnloadAll() {
	elementMap.clear();
}

void BattleManager::UnloadIf(std::function<bool(std::pair<const int, Battle const&>)> fn) {
	std::map<int, Battle>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

Battle* BattleManager::Find(int uid) {
	std::map<int, Battle>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int BattleManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, Battle>* BattleManager::GetContainer() {
	return &elementMap;
}

lua_State* BattleManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* BattleManager::GetLuaState() {
	return lua;
}

sqlite3* BattleManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* BattleManager::GetDatabase() {
	return database;
}
