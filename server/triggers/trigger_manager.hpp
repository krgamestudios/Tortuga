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
#ifndef TRIGGERMANAGER_HPP_
#define TRIGGERMANAGER_HPP_

#include "bounding_box.hpp"
#include "vector2.hpp"
#include "trigger_data.hpp"

#include "lua.hpp"

#include <functional>
#include <map>
#include <string>

class TriggerManager {
public:
	TriggerManager();
	~TriggerManager();

	//common public methods
	int Create();
	int Create(Vector2 origin, BoundingBox bounds);
	void Unload(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, TriggerData const&>)> fn);

	//accessors & mutators
	TriggerData* Get(int uid);
	int GetLoadedCount();
	std::map<int, TriggerData>* GetContainer();

	//hooks
	lua_State* SetLuaState(lua_State* L);
	lua_State* GetLuaState();

private:
	//members
	std::map<int, TriggerData> elementMap;
	lua_State* lua = nullptr;
	int counter = 0;
};

#endif