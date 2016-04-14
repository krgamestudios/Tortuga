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

#include "barrier_defines.hpp"

BarrierManager::BarrierManager() {
	//EMPTY
}

BarrierManager::~BarrierManager() {
	UnloadAll();
}

//arg: a list of barriers to be updated in the clients
void BarrierManager::Update(
		std::list<std::tuple<const int, BarrierData*, int>>* barrierList,
		std::list<std::tuple<const int, CreatureData*, int>>* creatureList,
		std::list<CharacterData*>* characterList
	)
{
	//for each given creature, if a collision was detected, make a new barrier
	for (auto& it : *creatureList) {
		if (std::get<2>(it) & 2) {
			int index = Create(-1); //instance from creature index?
			BarrierData* barrierData = Find(index);
			barrierData->SetOrigin({
				(CREATURE_BOUNDS_WIDTH - BARRIER_BOUNDS_WIDTH) / 2 + std::get<1>(it)->GetOrigin().x,
 				(CREATURE_BOUNDS_HEIGHT - BARRIER_BOUNDS_HEIGHT) / 2 + std::get<1>(it)->GetOrigin().y
		});
		}
	}

	//TODO: merge barriers
	//TODO: absorb creatures into existing barriers

	//update the barriers
	//TODO: how to delete the barriers?
	int ret; //0 = no action, ret&1 = update clients, ret&2 = collision detected
	for (auto& it : elementMap) {
		//normal update
		ret = it.second.Update(lua) ? 1 : 0;

		//check for collision with a character
		BoundingBox barrierBox = it.second.GetRealBounds();
		for (auto& it : *characterList) {
			if (barrierBox.CheckOverlap(it->GetRealBounds())) {
				//this will need updating
				ret |= 2;
			}

			//TODO: absorb characters
		}

		if (ret) {
			//push to the return list
			barrierList->push_back(std::make_tuple(it.first, &it.second, ret));
		}
	}
}

void BarrierManager::Cleanup(std::list<std::tuple<const int, BarrierData*, int>>* barrierList) {
	//unload the given barrier objects
	for (auto& it : *barrierList) {
		if (std::get<2>(it) & 4) {
			Unload(std::get<0>(it));
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
