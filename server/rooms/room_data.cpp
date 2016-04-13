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
#include "room_data.hpp"

#include "barrier_defines.hpp"
#include "culling_defines.hpp"
#include "serial_packet.hpp"
#include "server_utilities.hpp"

#include <algorithm>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <tuple>

//TODO: (9) character collisions should be preformed client-side
void RoomData::RunFrame() {
	//get the hook
	lua_rawgeti(lua, LUA_REGISTRYINDEX, tickRef);

	if (!lua_isnil(lua, -1)) {
		//call the tick function, with this as a parameter
		lua_pushlightuserdata(lua, this);
		if (lua_pcall(lua, 1, 0, 0) != LUA_OK) {
			throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(lua, -1) ));
		}
	}
	else {
		lua_pop(lua, 1);
	}

	//lists of non-character entities that need updating client-side
	//types are index, ptr, action (0 = update, 1 = unload)
	std::list<std::tuple<const int, CreatureData*, int>> creatureList;
	std::list<std::tuple<const int, BarrierData*, int>> barrierList;

	//update the entities in the room
	for (auto& it : characterList) {
		it->Update();
	}
	creatureMgr.Update(&creatureList, &characterList);
	barrierMgr.Update(&barrierList, &creatureList, &characterList);

	//send the creature updates
	for (auto& it : creatureList) {
		CreaturePacket packet;
		copyCreatureToPacket(&packet, std::get<1>(it), std::get<0>(it));
		packet.type = std::get<2>(it) != 0 ? SerialPacketType::CREATURE_UPDATE : SerialPacketType::CREATURE_UNLOAD;
		packet.roomIndex = roomIndex;
		pumpPacketProximity(reinterpret_cast<SerialPacket*>(&packet), roomIndex, std::get<1>(it)->GetOrigin(), INFLUENCE_RADIUS);
	}

	//send the barrier updates
	for (auto& it : barrierList) {
		BarrierPacket packet;
		copyBarrierToPacket(&packet, std::get<1>(it), std::get<0>(it));
		packet.type = std::get<2>(it) != 0 ? SerialPacketType::BARRIER_UPDATE : SerialPacketType::BARRIER_UNLOAD;
		packet.roomIndex = roomIndex;
		pumpPacketProximity(reinterpret_cast<SerialPacket*>(&packet), roomIndex, std::get<1>(it)->GetOrigin(), INFLUENCE_RADIUS);
	}

	//cleanup the lists
	creatureMgr.Cleanup(&creatureList);
	barrierMgr.Cleanup(&barrierList);

	//build a list of entities for use with the triggers
	std::stack<Entity*> entityStack;
	for (auto& it : characterList) {
		entityStack.push(it);
	}

	//Compare the triggers to the entities, using their real hitboxes
	triggerMgr.Compare(entityStack);
}

std::string RoomData::SetName(std::string s) {
	return roomName = s;
}

std::string RoomData::GetName() {
	return roomName;
}

std::string RoomData::SetTileset(std::string s) {
	return tilesetName = s;
}

std::string RoomData::GetTileset() {
	return tilesetName;
}

int RoomData::SetRoomIndex(int i) {
	return roomIndex = i;
}

int RoomData::GetRoomIndex() {
	return roomIndex;
}

BarrierManager* RoomData::GetBarrierMgr() {
	return &barrierMgr;
}

std::list<CharacterData*>* RoomData::GetCharacterList() {
	return &characterList;
}

CreatureManager* RoomData::GetCreatureMgr() {
	return &creatureMgr;
}

RegionPagerLua* RoomData::GetPager() {
	return &pager;
}

TriggerManager* RoomData::GetTriggerMgr() {
	return &triggerMgr;
}

lua_State* RoomData::SetLuaState(lua_State* L) {
	lua = L;
	barrierMgr.SetLuaState(lua);
	creatureMgr.SetLuaState(lua);
	pager.SetLuaState(lua);
	triggerMgr.SetLuaState(lua);
	return lua;
}

lua_State* RoomData::GetLuaState() {
	return lua;
}

sqlite3* RoomData::SetDatabase(sqlite3* db) {
	//TODO: (1) set database here
	database = db;
	return database;
}

sqlite3* RoomData::GetDatabase() {
	return database;
}

int RoomData::SetTickReference(int i) {
	return tickRef = i;
}

int RoomData::GetTickReference() {
	return tickRef;
}