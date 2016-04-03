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
#include "combat_instance_manager.hpp"

CombatInstanceManager::CombatInstanceManager() {
	//EMPTY
}

CombatInstanceManager::~CombatInstanceManager() {
	UnloadAll();
}

//arg: a list of combats to be updated in the clients
void CombatInstanceManager::Update() {
	for (auto& it : elementMap) {
		it.second.Update();
	}
}

int CombatInstanceManager::Create() {
	//implicitly create the new object
	elementMap.emplace( std::pair<int, CombatInstance>(counter, CombatInstance()) );

	//TODO: do various things like saving to the database
	return counter++;
}

//TODO: (1) combat load, save

void CombatInstanceManager::Unload(int uid) {
	elementMap.erase(uid);
}

void CombatInstanceManager::UnloadAll() {
	elementMap.clear();
}

void CombatInstanceManager::UnloadIf(std::function<bool(std::pair<const int, CombatInstance const&>)> fn) {
	std::map<int, CombatInstance>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

CombatInstance* CombatInstanceManager::Find(int uid) {
	std::map<int, CombatInstance>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int CombatInstanceManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, CombatInstance>* CombatInstanceManager::GetContainer() {
	return &elementMap;
}

lua_State* CombatInstanceManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* CombatInstanceManager::GetLuaState() {
	return lua;
}

sqlite3* CombatInstanceManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* CombatInstanceManager::GetDatabase() {
	return database;
}
