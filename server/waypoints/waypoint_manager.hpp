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
#ifndef WAYPOINTMANAGER_HPP_
#define WAYPOINTMANAGER_HPP_

#include "waypoint_data.hpp"
#include "singleton.hpp"
#include "vector2.hpp"

#if defined(__MINGW32__)
 #include "lua/lua.hpp"
#else
 #include "lua.hpp"
#endif

#include <functional>
#include <map>
#include <string>

//TODO: should waypoints be managed on a per-room basis?
class WaypointManager: public Singleton<WaypointManager> {
public:
	//common public methods
	int Create();
	int Load();
	int Save(int uid);
	void Unload(int uid);
	void Delete(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, WaypointData>)> fn);

	//accessors & mutators
	WaypointData* Get(int uid);
	int GetLoadedCount();
	int GetTotalCount();
	std::map<int, WaypointData>* GetContainer();

	//hooks
	lua_State* SetLuaState(lua_State* L) { return lua = L; }
	lua_State* GetLuaState() { return lua; }

private:
	friend Singleton<WaypointManager>;

	WaypointManager() = default;
	~WaypointManager() = default;

	//members
	std::map<int, WaypointData> elementMap;
	lua_State* lua = nullptr;
	int counter = 0;
};

#endif