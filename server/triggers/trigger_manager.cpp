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
#include "trigger_manager.hpp"

TriggerManager::TriggerManager() {
	//EMPTY
}

TriggerManager::~TriggerManager() {
	UnloadAll();
}

//Compare the triggers to the entities, using their real hitboxes
void TriggerManager::Compare(std::stack<Entity*> entityStack) {
	//NOTE: this stack solution should prevent problems when modifying the various lists
	while(entityStack.size()) {
		//get the entity & hitbox
		Entity* entity = entityStack.top();
		BoundingBox entityBox = entity->GetBounds() + entity->GetOrigin();

		//get the trigger pair & hitbox
		for (auto& triggerPair : elementMap) {
			BoundingBox triggerBox = triggerPair.second.GetBoundingBox() + triggerPair.second.GetOrigin();

			//find all collisions
			if (entityBox.CheckOverlap(triggerBox)) {
				//skip members of the exclusion list
				if (std::any_of(triggerPair.second.GetExclusionList()->begin(), triggerPair.second.GetExclusionList()->end(), [entity](Entity* ptr) -> bool {
					return entity == ptr;
				})) {
					continue;
				}

				//push to the exclusion list
				triggerPair.second.GetExclusionList()->push_back(entity);

				//run the trigger script
				lua_rawgeti(lua, LUA_REGISTRYINDEX, triggerPair.second.GetScriptReference());
				lua_pushlightuserdata(lua, entity);

				if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
					//error
					throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
				}
			}
			else {
				//remove members of the exclusion list
				//NOTE: characters in different rooms won't be removed, which does tend to be a problem
				triggerPair.second.GetExclusionList()->remove_if([entity](Entity* ptr) -> bool {
					return entity == ptr;
				});
			}
		}

		//next
		entityStack.pop();
	}
}

int TriggerManager::Create(std::string handle) {
	//implicitly creates the element
	TriggerData& triggerData = elementMap[counter];

	triggerData.SetHandle(handle);

	return counter++;
}

void TriggerManager::Unload(int uid) {
	elementMap.erase(uid);
}

void TriggerManager::UnloadAll() {
	//TODO: save?
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

TriggerData* TriggerManager::Find(int uid) {
	std::map<int, TriggerData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

TriggerData* TriggerManager::Find(std::string handle) {
	for (std::map<int, TriggerData>::iterator it = elementMap.begin(); it != elementMap.end(); ++it) {
		if (it->second.GetHandle() == handle) {
			return &it->second;
		}
	}
	return nullptr;
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