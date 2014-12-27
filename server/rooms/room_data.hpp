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
#ifndef ROOMDATA_HPP_
#define ROOMDATA_HPP_

#include "entity.hpp"
#include "region_pager_lua.hpp"

#if defined(__MINGW32__)
 #include "lua/lua.hpp"
#else
 #include "lua.hpp"
#endif

#include <list>
#include <string>

class RoomData {
public:
	RoomData() = default;
	~RoomData() = default;

	//accessors and mutators
	std::string SetRoomName(std::string s);
	std::string GetRoomName();

	std::string SetTilesetName(std::string s);
	std::string GetTilesetName();

	RegionPagerLua* GetPager();
	std::list<Entity*>* GetEntityList();

private:
	friend class RoomManager;

	//members
	std::string roomName;
	std::string tilesetName;

	RegionPagerLua pager;
	std::list<Entity*> entityList;
};

#endif
