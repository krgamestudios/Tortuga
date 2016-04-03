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

#include "culling_defines.hpp"
#include "serial_packet.hpp"
#include "server_utilities.hpp"

#include <algorithm>
#include <iostream>
#include <stack>
#include <stdexcept>

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
	std::list<std::pair<const int, CreatureData*>> creatureList;
	std::list<std::pair<const int, BarrierData*>> barrierList;

	//update the entities in the room
	for (auto& it : characterList) {
		it->Update();
	}
	creatureMgr.Update(&creatureList);
	barrierMgr.Update(&barrierList);

	//build a list of entities for use with the triggers
	std::stack<Entity*> entityStack;
	for (auto& it : characterList) {
		entityStack.push(it);
	}

	//Compare the triggers to the entities, using their real hitboxes
	triggerMgr.Compare(entityStack);

	//Creature/character collisions, O(m*n)
	for (auto characterIt : characterList) {
		BoundingBox characterBox = characterIt->GetBounds() + characterIt->GetOrigin();

		for (auto creatureIt : *creatureMgr.GetContainer()) {
			BoundingBox creatureBox = creatureIt.second.GetBounds() + creatureIt.second.GetOrigin();

			if (characterBox.CheckOverlap(creatureBox)) {
				int barrierIndex = barrierMgr.Create(-1);
				BarrierData* barrierData = barrierMgr.Find(barrierIndex);
				barrierData->SetRoomIndex(roomIndex);

				BarrierPacket barrierPacket;
				barrierPacket.type = SerialPacketType::BARRIER_CREATE;
				copyBarrierToPacket(&barrierPacket, barrierData, barrierIndex);

				pumpPacketProximity(reinterpret_cast<SerialPacket*>(&barrierPacket), roomIndex, characterIt->GetOrigin(), INFLUENCE_RADIUS);
			}
		}
	}

	//send the creature updates
	for (auto& it : creatureList) {
		CreaturePacket packet;
		copyCreatureToPacket(&packet, it.second, it.first);
		packet.type = SerialPacketType::CREATURE_UPDATE;
		packet.roomIndex = roomIndex;
		pumpPacketProximity(reinterpret_cast<SerialPacket*>(&packet), roomIndex, it.second->GetOrigin(), INFLUENCE_RADIUS);
	}

	//send the barrier updates
	for (auto& it : barrierList) {
		BarrierPacket packet;
		copyBarrierToPacket(&packet, it.second, it.first);
		packet.type = SerialPacketType::BARRIER_UPDATE;
		packet.roomIndex = roomIndex;
		pumpPacketProximity(reinterpret_cast<SerialPacket*>(&packet), roomIndex, it.second->GetOrigin(), INFLUENCE_RADIUS);
	}
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
	creatureMgr.SetLuaState(lua);
	pager.SetLuaState(lua);
	triggerMgr.SetLuaState(lua);
	return lua;
}

lua_State* RoomData::GetLuaState() {
	return lua;
}

sqlite3* RoomData::SetDatabase(sqlite3* db) {
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