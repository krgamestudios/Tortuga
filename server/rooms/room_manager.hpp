/* Copyright: (c) Kayne Ruse 2014
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
#ifndef ROOMMANAGER_HPP_
#define ROOMMANAGER_HPP_

#include "room_data.hpp"
#include "singleton.hpp"
#include "manager_interface.hpp"

#if defined(__MINGW32__)
 #include "lua/lua.hpp"
#else
 #include "lua.hpp"
#endif

class RoomManager:
	public Singleton<RoomManager>,
	public ManagerInterface<RoomData, std::string>
{
public:
	//common public methods
	int Create(std::string) override;
	void Unload(int uid) override;

	void UnloadAll() override;
	void UnloadIf(std::function<bool(std::pair<const int,RoomData>)> fn) override;

	//accessors and mutators
	RoomData* Get(int uid) override;
	int GetLoadedCount() override;
	int GetTotalCount() override;
	std::map<int, RoomData>* GetContainer() override;

	//hooks
	lua_State* SetLuaState(lua_State* L) { return lua = L; }
	lua_State* GetLuaState() { return lua; }

private:
	friend Singleton<RoomManager>;

	RoomManager() = default;
	~RoomManager() = default;

	//EMPTY
	int Load(std::string) override { return -1; }
	int Save(int uid) override { return -1; }
	void Delete(int uid) override {}

	lua_State* lua = nullptr;
	int counter = 0;
};

#endif
