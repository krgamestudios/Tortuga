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

#include "character_data.hpp"
#include "monster_manager.hpp"
#include "region_pager_lua.hpp"
#include "waypoint_manager.hpp"

#include "lua.hpp"

#include <list>
#include <string>

class RoomData {
public:
	RoomData() = default;
	~RoomData() = default;

	//accessors and mutators
	std::string SetName(std::string);
	std::string GetName();

	std::string SetTileset(std::string);
	std::string GetTileset();

	RegionPagerLua* GetPager();
	MonsterManager* GetMonsterMgr();
	WaypointManager* GetWaypointMgr();
	std::list<CharacterData*>* GetCharacterList();

	void RunFrame();

	//TODO: triggers for unload, save, per-second, player enter, player exit, etc.

private:
	friend class RoomManager;

	//members
	std::string roomName;
	std::string tilesetName;

	RegionPagerLua pager;
	MonsterManager monsterMgr;
	WaypointManager waypointMgr;
	std::list<CharacterData*> characterList;
};

#endif
