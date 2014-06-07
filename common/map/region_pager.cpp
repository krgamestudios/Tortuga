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
#include "region_pager.hpp"

#include "utility.hpp"

#include <stdexcept>

Region::type_t RegionPager::SetTile(int x, int y, int z, Region::type_t v) {
	Region* ptr = GetRegion(x, y);
	return ptr->SetTile(x - ptr->GetX(), y - ptr->GetY(), z, v);
}

Region::type_t RegionPager::GetTile(int x, int y, int z) {
	Region* ptr = GetRegion(x, y);
	return ptr->GetTile(x - ptr->GetX(), y - ptr->GetY(), z);
}

Region* RegionPager::GetRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//get the region by various means
	Region* ptr = nullptr;
	ptr = FindRegion(x, y);
	if (ptr) return ptr;
	ptr = LoadRegion(x, y);
	if (ptr) return ptr;
	return CreateRegion(x, y);
}

Region* RegionPager::FindRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//find the region
	for (std::list<Region*>::iterator it = regionList.begin(); it != regionList.end(); it++) {
		if ((*it)->GetX() == x && (*it)->GetY() == y) {
			return *it;
		}
	}
	return nullptr;
}

Region* RegionPager::LoadRegion(int x, int y) {
	//load the region if possible

	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	Region* ptr = new Region(x, y);

	//API hook
	lua_getglobal(luaState, "map");
	lua_getfield(luaState, -1, "load");
	lua_pushlightuserdata(luaState, ptr);
	lua_pushstring(luaState, directory.c_str());
	if (lua_pcall(luaState, 2, 1, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	if (lua_toboolean(luaState, -1)) {
		regionList.push_front(ptr);
	}
	else {
		delete ptr;
		ptr = nullptr;
	}
	lua_pop(luaState, 2);

	return ptr;
}

Region* RegionPager::SaveRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//find & save the region
	Region* ptr = FindRegion(x, y);
	if (ptr) {
		//API hook
		lua_getglobal(luaState, "map");
		lua_getfield(luaState, -1, "save");
		lua_pushlightuserdata(luaState, ptr);
		lua_pushstring(luaState, directory.c_str());
		if (lua_pcall(luaState, 2, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
		}
		lua_pop(luaState, 1);
	}
	return ptr;
}

Region* RegionPager::CreateRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//create and push the object
	Region* ptr = new Region(x, y);

	//API hook
	lua_getglobal(luaState, "map");
	lua_getfield(luaState, -1, "create");
	lua_pushlightuserdata(luaState, ptr);
	if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
	}
	lua_pop(luaState, 1);

	regionList.push_front(ptr);
	return ptr;
}

void RegionPager::UnloadRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	lua_getglobal(luaState, "map");

	//custom loop, not FindRegion()
	for (std::list<Region*>::iterator it = regionList.begin(); it != regionList.end(); /* EMPTY */) {
		if ((*it)->GetX() == x && (*it)->GetY() == y) {

			//API hook
			lua_getfield(luaState, -1, "unload");
			lua_pushlightuserdata(luaState, *it);
			if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {
				throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(luaState, -1) ));
			}

			delete (*it);
			it = regionList.erase(it);
			continue;
		}
		++it;
	}

	lua_pop(luaState, 1);
}
