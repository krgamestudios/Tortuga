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
#include "barrier_manager.hpp"

#include "lua_utilities.hpp"

BarrierManager::BarrierManager() {
	//EMPTY
}

BarrierManager::~BarrierManager() {
	UnloadAll();
}

//arg: a list of barriers to be updated in the clients
void BarrierManager::Update(std::list<std::pair<const int, BarrierData*>>* barrierList, bool updateAll) {
	int ret;
	for (auto& it : elementMap) {
		ret = it.second.Update(lua);
		if (ret || updateAll) {
			barrierList->push_back(std::pair<const int, BarrierData*>(it.first, &it.second));
		}
	}
}

int BarrierManager::Create(int instanceIndex) {
	//implicitly create the new object
	elementMap.emplace( std::pair<int, BarrierData>(counter, BarrierData(instanceIndex)) );

	runHook(lua, createRef, &elementMap.find(counter)->second, counter);

	//TODO: do various things like saving to the database
	return counter++;
}

//TODO: (1) combat load, save

void BarrierManager::Unload(int uid) {
	runHook(lua, unloadRef, &elementMap.find(uid)->second, uid);
	elementMap.erase(uid);
}

void BarrierManager::UnloadAll() {
	for (std::map<int, BarrierData>::iterator it = elementMap.begin(); it != elementMap.end(); it++) {
		runHook(lua, unloadRef, &it->second, it->first);
	}
	elementMap.clear();
}

void BarrierManager::UnloadIf(std::function<bool(std::pair<const int, BarrierData const&>)> fn) {
	std::map<int, BarrierData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			runHook(lua, unloadRef, &it->second, it->first);
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

BarrierData* BarrierManager::Find(int uid) {
	std::map<int, BarrierData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int BarrierManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, BarrierData>* BarrierManager::GetContainer() {
	return &elementMap;
}

lua_State* BarrierManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* BarrierManager::GetLuaState() {
	return lua;
}

sqlite3* BarrierManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* BarrierManager::GetDatabase() {
	return database;
}

int BarrierManager::SetCreateReference(int i) {
	return createRef = i;
}

int BarrierManager::SetUnloadReference(int i) {
	return unloadRef = i;
}

int BarrierManager::GetCreateReference() {
	return createRef;
}

int BarrierManager::GetUnloadReference() {
	return unloadRef;
}
