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
#include "server_application.hpp"

#include <iostream>
#include <sstream>

//-------------------------
//Queries
//-------------------------

void ServerApplication::hRegionRequest(RegionPacket* const argPacket) {
	//get the region object, send a rejection on error
	RoomData* room = roomMgr.Get(argPacket->roomIndex);
	if (!room) {
		//build the error message
		std::ostringstream msg;
		msg << "Failed to find Region (" << argPacket->roomIndex << "," << argPacket->x << "," << argPacket->y << "); ";
		msg << "Room " << argPacket->roomIndex << " does not exist";

		//build the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::REGION_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		//log the error
		std::cerr << "Error message sent: " << newPacket.text << std::endl;
		return;
	}
	Region* region = room->GetPager()->GetRegion(argPacket->x, argPacket->y);

	//send the content
	RegionPacket newPacket;

	newPacket.type = SerialPacketType::REGION_CONTENT;
	newPacket.roomIndex = argPacket->roomIndex;
	newPacket.x = argPacket->x;
	newPacket.y = argPacket->y;
	newPacket.region = region;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::hQueryCharacterExists(CharacterPacket* const argPacket) {
	//respond with all character data
	CharacterPacket newPacket;

	for (auto& it : *characterMgr.GetContainer()) {
		if (argPacket->roomIndex != -1 && it.second.GetRoomIndex() != argPacket->roomIndex) {
			continue;
		}
		CopyCharacterToPacket(&newPacket, it.first);
		newPacket.type = SerialPacketType::QUERY_CHARACTER_EXISTS;
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
	}
}

void ServerApplication::hQueryCharacterStats(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

void ServerApplication::hQueryCharacterLocation(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

void ServerApplication::hQueryMonsterExists(MonsterPacket* const argPacket) {
	//TODO: (9) empty
}

void ServerApplication::hQueryMonsterStats(MonsterPacket* const argPacket) {
	//TODO: (9) empty
}

void ServerApplication::hQueryMonsterLocation(MonsterPacket* const argPacket) {
	//TODO: (9) empty
}
