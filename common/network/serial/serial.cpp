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
#include "serial.hpp"

#include "serial_util.hpp"

//simple type functions
void serializeType(SerialPacketBase* packet, void* buffer) {
	SERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));
}

void deserializeType(SerialPacketBase* packet, void* buffer) {
	DESERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));
}

//main switch functions
void serializePacket(SerialPacketBase* packet, void* buffer) {
	switch(packet->type) {
		//no extra data
		case SerialPacketType::NONE:
		case SerialPacketType::PING:
		case SerialPacketType::PONG:
		case SerialPacketType::BROADCAST_REQUEST:

		//all rejections
		case SerialPacketType::JOIN_REJECTION:
		case SerialPacketType::CHARACTER_REJECTION:
		case SerialPacketType::ENEMY_REJECTION:
		case SerialPacketType::COMBAT_REJECTION:

			serializeType(packet, buffer);
		break;

		//character info
		case SerialPacketType::CHARACTER_NEW:
		case SerialPacketType::CHARACTER_DELETE:
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
		case SerialPacketType::CHARACTER_STATS_RESPONSE:
			serializeCharacter(static_cast<CharacterPacket*>(packet), buffer);
		break;

		//client info
		case SerialPacketType::JOIN_REQUEST:
		case SerialPacketType::JOIN_RESPONSE:
		case SerialPacketType::SYNCHRONIZE:
		case SerialPacketType::DISCONNECT:
		case SerialPacketType::SHUTDOWN:
			serializeClient(static_cast<ClientPacket*>(packet), buffer);
		break;

		//combat info
		case SerialPacketType::COMBAT_NEW:
		case SerialPacketType::COMBAT_DELETE:
		case SerialPacketType::COMBAT_UPDATE:

		case SerialPacketType::COMBAT_ENTER_REQUEST:
		case SerialPacketType::COMBAT_ENTER_RESPONSE:
		case SerialPacketType::COMBAT_EXIT_REQUEST:
		case SerialPacketType::COMBAT_EXIT_RESPONSE:

			serializeCombat(static_cast<CombatPacket*>(packet), buffer);
		break;

		//enemy info
		case SerialPacketType::ENEMY_NEW:
		case SerialPacketType::ENEMY_DELETE:
		case SerialPacketType::ENEMY_UPDATE:
		case SerialPacketType::ENEMY_STATS_REQUEST:
		case SerialPacketType::ENEMY_STATS_RESPONSE:
			serializeEnemy(static_cast<EnemyPacket*>(packet), buffer);
		break;

		//region info
		case SerialPacketType::REGION_REQUEST:
			serializeRegionFormat(static_cast<RegionPacket*>(packet), buffer);
		break;

		case SerialPacketType::REGION_CONTENT:
			serializeRegionContent(static_cast<RegionPacket*>(packet), buffer);
		break;

		//server info
		case SerialPacketType::BROADCAST_RESPONSE:
			serializeServer(static_cast<ServerPacket*>(packet), buffer);
		break;
	}
}

void deserializePacket(SerialPacketBase* packet, void* buffer) {
	//find the type, so that you can actually deserialize the packet!
	deserializeType(packet, buffer);
	switch(packet->type) {
		//no extra data
		case SerialPacketType::NONE:
		case SerialPacketType::PING:
		case SerialPacketType::PONG:
		case SerialPacketType::BROADCAST_REQUEST:

		//all rejections
		case SerialPacketType::JOIN_REJECTION:
		case SerialPacketType::CHARACTER_REJECTION:
		case SerialPacketType::ENEMY_REJECTION:
		case SerialPacketType::COMBAT_REJECTION:

			//NOTHING
		break;

		//character info
		case SerialPacketType::CHARACTER_NEW:
		case SerialPacketType::CHARACTER_DELETE:
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
		case SerialPacketType::CHARACTER_STATS_RESPONSE:
			deserializeCharacter(static_cast<CharacterPacket*>(packet), buffer);
		break;

		//client info
		case SerialPacketType::JOIN_REQUEST:
		case SerialPacketType::JOIN_RESPONSE:
		case SerialPacketType::SYNCHRONIZE:
		case SerialPacketType::DISCONNECT:
		case SerialPacketType::SHUTDOWN:
			deserializeClient(static_cast<ClientPacket*>(packet), buffer);
		break;

		//combat info
		case SerialPacketType::COMBAT_NEW:
		case SerialPacketType::COMBAT_DELETE:
		case SerialPacketType::COMBAT_UPDATE:

		//TODO: is this the best fit?
		case SerialPacketType::COMBAT_ENTER_REQUEST:
		case SerialPacketType::COMBAT_ENTER_RESPONSE:
		case SerialPacketType::COMBAT_EXIT_REQUEST:
		case SerialPacketType::COMBAT_EXIT_RESPONSE:

			serializeCombat(static_cast<CombatPacket*>(packet), buffer);
		break;

		//enemy info
		case SerialPacketType::ENEMY_NEW:
		case SerialPacketType::ENEMY_DELETE:
		case SerialPacketType::ENEMY_UPDATE:
		case SerialPacketType::ENEMY_STATS_REQUEST:
		case SerialPacketType::ENEMY_STATS_RESPONSE:
			serializeEnemy(static_cast<EnemyPacket*>(packet), buffer);
		break;

		//region info
		case SerialPacketType::REGION_REQUEST:
			deserializeRegionFormat(static_cast<RegionPacket*>(packet), buffer);
		break;

		case SerialPacketType::REGION_CONTENT:
			deserializeRegionContent(static_cast<RegionPacket*>(packet), buffer);
		break;

		//server info
		case SerialPacketType::BROADCAST_RESPONSE:
			deserializeServer(static_cast<ServerPacket*>(packet), buffer);
		break;
	}
}