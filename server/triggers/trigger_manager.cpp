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
#include "trigger_manager.hpp"

TriggerManager::TriggerManager() {
	//EMPTY
}

TriggerManager::~TriggerManager() {
	UnloadAll();
}

int TriggerManager::Create() {
	//implicitly creates the element
	TriggerData& triggerData = elementMap[counter];

	//no real values set
	triggerData.origin = {0, 0};
	triggerData.bounds = {0, 0, 0, 0};

	return counter++;
}

int TriggerManager::Create(Vector2 origin, BoundingBox bounds) {
	//implicitly creates the element
	TriggerData& triggerData = elementMap[counter];

	triggerData.origin = origin;
	triggerData.bounds = bounds;

	return counter++;
}

void TriggerManager::Unload(int uid) {
	elementMap.erase(uid);
}

void TriggerManager::UnloadAll() {
	elementMap.clear();
}

void TriggerManager::UnloadIf(std::function<bool(std::pair<const int, TriggerData const&>)> fn) {
	std::map<int, TriggerData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

TriggerData* TriggerManager::Get(int uid) {
	std::map<int, TriggerData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int TriggerManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, TriggerData>* TriggerManager::GetContainer() {
	return &elementMap;
}

//hooks
lua_State* TriggerManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* TriggerManager::GetLuaState() {
	return lua;
}