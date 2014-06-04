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

#include "map_allocator.hpp"
#include "statistics.hpp"

#include <cstring>

//-------------------------
//Convenience Macros
//-------------------------

#define SERIALIZE(buffer, data, size) memcpy(buffer, data, size); buffer += size;
#define DESERIALIZE(buffer, data, size) memcpy(data, buffer, size); buffer += size;

//-------------------------
//internal serialization functions
//-------------------------

void serializeType(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));
}

void serializeServer(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//server info
	SERIALIZE(buffer, &packet->serverInfo.networkVersion, sizeof(int));
	SERIALIZE(buffer, packet->serverInfo.name, PACKET_STRING_SIZE);
	SERIALIZE(buffer, &packet->serverInfo.playerCount, sizeof(int));
}

void serializeClient(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//indexes
	SERIALIZE(buffer, &packet->clientInfo.clientIndex, sizeof(int));
	SERIALIZE(buffer, &packet->clientInfo.accountIndex, sizeof(int));
	SERIALIZE(buffer, &packet->clientInfo.characterIndex, sizeof(int));

	//texts
	SERIALIZE(buffer, packet->clientInfo.username, PACKET_STRING_SIZE);
	//TODO: password
	SERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	SERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);
}

void serializeRegionFormat(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//format
	SERIALIZE(buffer, &packet->regionInfo.mapIndex, sizeof(int));
	SERIALIZE(buffer, &packet->regionInfo.x, sizeof(int));
	SERIALIZE(buffer, &packet->regionInfo.y, sizeof(int));
}

void serializeRegionContent(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//format
	SERIALIZE(buffer, &packet->regionInfo.mapIndex, sizeof(int));
	SERIALIZE(buffer, &packet->regionInfo.x, sizeof(int));
	SERIALIZE(buffer, &packet->regionInfo.y, sizeof(int));

	//content
	for (register int i = 0; i < REGION_WIDTH; i++) {
		for (register int j = 0; j < REGION_HEIGHT; j++) {
			for (register int k = 0; k < REGION_DEPTH; k++) {
				*reinterpret_cast<Region::type_t*>(buffer) = packet->regionInfo.region->GetTile(i, j, k);
				buffer += sizeof(Region::type_t);
			}
		}
	}
}

void serializeCombat(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//integers
	SERIALIZE(buffer, &packet->combatInfo.combatIndex, sizeof(int));
	SERIALIZE(buffer, &packet->combatInfo.difficulty, sizeof(int));

	SERIALIZE(buffer, &packet->combatInfo.terrainType, sizeof(CombatData::Terrain));

	//arrays
	SERIALIZE(buffer, &packet->combatInfo.characterArray, COMBAT_MAX_CHARACTERS);
	SERIALIZE(buffer, &packet->combatInfo.enemyArray, COMBAT_MAX_ENEMIES);

	//position
	SERIALIZE(buffer, &packet->combatInfo.mapIndex, sizeof(int));
	SERIALIZE(buffer, &packet->combatInfo.origin.x, sizeof(double));
	SERIALIZE(buffer, &packet->combatInfo.origin.y, sizeof(double));

	//TODO: rewards
}

void serializeStatistics(Statistics* stats, char* buffer) {
	//integers
	SERIALIZE(buffer, &stats->level, sizeof(int));
	SERIALIZE(buffer, &stats->exp, sizeof(int));
	SERIALIZE(buffer, &stats->maxHP, sizeof(int));
	SERIALIZE(buffer, &stats->health, sizeof(int));
	SERIALIZE(buffer, &stats->maxMP, sizeof(int));
	SERIALIZE(buffer, &stats->mana, sizeof(int));
	SERIALIZE(buffer, &stats->attack, sizeof(int));
	SERIALIZE(buffer, &stats->defence, sizeof(int));
	SERIALIZE(buffer, &stats->intelligence, sizeof(int));
	SERIALIZE(buffer, &stats->resistance, sizeof(int));
	SERIALIZE(buffer, &stats->speed, sizeof(int));

	//floats
	SERIALIZE(buffer, &stats->accuracy, sizeof(float));
	SERIALIZE(buffer, &stats->evasion, sizeof(float));
	SERIALIZE(buffer, &stats->luck, sizeof(float));
}

void serializeCharacter(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//indexes
	SERIALIZE(buffer, &packet->characterInfo.clientIndex, sizeof(int));
	SERIALIZE(buffer, &packet->characterInfo.accountIndex, sizeof(int));
	SERIALIZE(buffer, &packet->characterInfo.characterIndex, sizeof(int));

	//texts
	SERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	SERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);

	//vectors
	SERIALIZE(buffer, &packet->characterInfo.origin.x, sizeof(double));
	SERIALIZE(buffer, &packet->characterInfo.origin.y, sizeof(double));
	SERIALIZE(buffer, &packet->characterInfo.motion.x, sizeof(double));
	SERIALIZE(buffer, &packet->characterInfo.motion.y, sizeof(double));

	//stats structure
	serializeStatistics(&packet->characterInfo.stats, buffer);
	buffer += sizeof(Statistics);
}

void serializeEnemy(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//texts
	SERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	SERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);

	//stats structure
	serializeStatistics(&packet->characterInfo.stats, buffer);
	buffer += sizeof(Statistics);
}

//-------------------------
//internal deserialization functions
//-------------------------

void deserializeType(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));
}

void deserializeServer(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//server info
	DESERIALIZE(buffer, &packet->serverInfo.networkVersion, sizeof(int));
	DESERIALIZE(buffer, packet->serverInfo.name, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, &packet->serverInfo.playerCount, sizeof(int));
}

void deserializeClient(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//indexes
	DESERIALIZE(buffer, &packet->clientInfo.clientIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->clientInfo.accountIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->clientInfo.characterIndex, sizeof(int));

	//texts
	DESERIALIZE(buffer, packet->clientInfo.username, PACKET_STRING_SIZE);
	//TODO: password
	DESERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);
}

void deserializeRegionFormat(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//format
	DESERIALIZE(buffer, &packet->regionInfo.mapIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->regionInfo.x, sizeof(int));
	DESERIALIZE(buffer, &packet->regionInfo.y, sizeof(int));
}

void deserializeRegionContent(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//format
	DESERIALIZE(buffer, &packet->regionInfo.mapIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->regionInfo.x, sizeof(int));
	DESERIALIZE(buffer, &packet->regionInfo.y, sizeof(int));

	//an object to work on
	BlankAllocator().Create(
		&packet->regionInfo.region,
		packet->regionInfo.x,
		packet->regionInfo.y
	);

	//content
	for (register int i = 0; i < REGION_WIDTH; i++) {
		for (register int j = 0; j < REGION_HEIGHT; j++) {
			for (register int k = 0; k < REGION_DEPTH; k++) {
				packet->regionInfo.region->SetTile(i, j, k, *reinterpret_cast<Region::type_t*>(buffer));
				buffer += sizeof(Region::type_t);
			}
		}
	}
}


void deserializeCombat(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//integers
	DESERIALIZE(buffer, &packet->combatInfo.combatIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->combatInfo.difficulty, sizeof(int));

	DESERIALIZE(buffer, &packet->combatInfo.terrainType, sizeof(CombatData::Terrain));

	//arrays
	DESERIALIZE(buffer, &packet->combatInfo.characterArray, COMBAT_MAX_CHARACTERS);
	DESERIALIZE(buffer, &packet->combatInfo.enemyArray, COMBAT_MAX_ENEMIES);

	//position
	DESERIALIZE(buffer, &packet->combatInfo.mapIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->combatInfo.origin.x, sizeof(double));
	DESERIALIZE(buffer, &packet->combatInfo.origin.y, sizeof(double));

	//TODO: rewards
}


void deserializeStatistics(Statistics* stats, char* buffer) {
	//integers
	DESERIALIZE(buffer, &stats->level, sizeof(int));
	DESERIALIZE(buffer, &stats->exp, sizeof(int));
	DESERIALIZE(buffer, &stats->maxHP, sizeof(int));
	DESERIALIZE(buffer, &stats->health, sizeof(int));
	DESERIALIZE(buffer, &stats->maxMP, sizeof(int));
	DESERIALIZE(buffer, &stats->mana, sizeof(int));
	DESERIALIZE(buffer, &stats->attack, sizeof(int));
	DESERIALIZE(buffer, &stats->defence, sizeof(int));
	DESERIALIZE(buffer, &stats->intelligence, sizeof(int));
	DESERIALIZE(buffer, &stats->resistance, sizeof(int));
	DESERIALIZE(buffer, &stats->speed, sizeof(int));

	//floats
	DESERIALIZE(buffer, &stats->accuracy, sizeof(float));
	DESERIALIZE(buffer, &stats->evasion, sizeof(float));
	DESERIALIZE(buffer, &stats->luck, sizeof(float));
}

void deserializeCharacter(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//indexes
	DESERIALIZE(buffer, &packet->characterInfo.clientIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->characterInfo.accountIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->characterInfo.characterIndex, sizeof(int));

	//texts
	DESERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);

	//vectors
	DESERIALIZE(buffer, &packet->characterInfo.origin.x, sizeof(double));
	DESERIALIZE(buffer, &packet->characterInfo.origin.y, sizeof(double));
	DESERIALIZE(buffer, &packet->characterInfo.motion.x, sizeof(double));
	DESERIALIZE(buffer, &packet->characterInfo.motion.y, sizeof(double));

	//stats structure
	deserializeStatistics(&packet->characterInfo.stats, buffer);
	buffer += sizeof(Statistics);
}

void deserializeEnemy(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//texts
	DESERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);

	//stats structure
	deserializeStatistics(&packet->characterInfo.stats, buffer);
	buffer += sizeof(Statistics);
}

//-------------------------
//the interface functions
//-------------------------

void serialize(SerialPacket* packet, void* buffer) {
	switch(packet->meta.type) {
		//no extra data
		case SerialPacket::Type::NONE:
		case SerialPacket::Type::PING:
		case SerialPacket::Type::PONG:
		case SerialPacket::Type::BROADCAST_REQUEST:

		//all rejections
		case SerialPacket::Type::BROADCAST_REJECTION:
		case SerialPacket::Type::JOIN_REJECTION:
		case SerialPacket::Type::REGION_REJECTION:
		case SerialPacket::Type::CHARACTER_REJECTION:
		case SerialPacket::Type::ENEMY_REJECTION:
		case SerialPacket::Type::COMBAT_REJECTION:

			serializeType(packet, reinterpret_cast<char*>(buffer));
		break;

		//server info
		case SerialPacket::Type::BROADCAST_RESPONSE:
			serializeServer(packet, reinterpret_cast<char*>(buffer));
		break;

		//client info
		case SerialPacket::Type::JOIN_REQUEST:
		case SerialPacket::Type::JOIN_RESPONSE:
		case SerialPacket::Type::SYNCHRONIZE:
		case SerialPacket::Type::DISCONNECT:
		case SerialPacket::Type::SHUTDOWN:
			serializeClient(packet, reinterpret_cast<char*>(buffer));
		break;

		//region info
		case SerialPacket::Type::REGION_REQUEST:
			serializeRegionFormat(packet, reinterpret_cast<char*>(buffer));
		break;

		case SerialPacket::Type::REGION_CONTENT:
			serializeRegionContent(packet, reinterpret_cast<char*>(buffer));
		break;

		//combat info
		case SerialPacket::Type::COMBAT_NEW:
		case SerialPacket::Type::COMBAT_DELETE:
		case SerialPacket::Type::COMBAT_UPDATE:

		//TODO: is this the best fit?
		case SerialPacket::Type::COMBAT_ENTER_REQUEST:
		case SerialPacket::Type::COMBAT_ENTER_RESPONSE:
		case SerialPacket::Type::COMBAT_EXIT_REQUEST:
		case SerialPacket::Type::COMBAT_EXIT_RESPONSE:

			serializeCombat(packet, reinterpret_cast<char*>(buffer));
		break;

		//character info
		case SerialPacket::Type::CHARACTER_NEW:
		case SerialPacket::Type::CHARACTER_DELETE:
		case SerialPacket::Type::CHARACTER_UPDATE:
		case SerialPacket::Type::CHARACTER_STATS_REQUEST:
		case SerialPacket::Type::CHARACTER_STATS_RESPONSE:
			serializeCharacter(packet, reinterpret_cast<char*>(buffer));
		break;

		//enemy info
		case SerialPacket::Type::ENEMY_NEW:
		case SerialPacket::Type::ENEMY_DELETE:
		case SerialPacket::Type::ENEMY_UPDATE:
		case SerialPacket::Type::ENEMY_STATS_REQUEST:
		case SerialPacket::Type::ENEMY_STATS_RESPONSE:
			serializeEnemy(packet, reinterpret_cast<char*>(buffer));
		break;
	}
}

void deserialize(SerialPacket* packet, void* buffer) {
	//find the type, so that you can actually deserialize the packet!
	deserializeType(packet, reinterpret_cast<char*>(buffer));
	switch(packet->meta.type) {
		//no extra data
		case SerialPacket::Type::NONE:
		case SerialPacket::Type::PING:
		case SerialPacket::Type::PONG:
		case SerialPacket::Type::BROADCAST_REQUEST:

		//all rejections
		case SerialPacket::Type::BROADCAST_REJECTION:
		case SerialPacket::Type::JOIN_REJECTION:
		case SerialPacket::Type::REGION_REJECTION:
		case SerialPacket::Type::CHARACTER_REJECTION:
		case SerialPacket::Type::ENEMY_REJECTION:
		case SerialPacket::Type::COMBAT_REJECTION:

			//NOTHING
		break;

		//server info
		case SerialPacket::Type::BROADCAST_RESPONSE:
			deserializeServer(packet, reinterpret_cast<char*>(buffer));
		break;

		//client info
		case SerialPacket::Type::JOIN_REQUEST:
		case SerialPacket::Type::JOIN_RESPONSE:
		case SerialPacket::Type::SYNCHRONIZE:
		case SerialPacket::Type::DISCONNECT:
		case SerialPacket::Type::SHUTDOWN:
			deserializeClient(packet, reinterpret_cast<char*>(buffer));
		break;

		//region info
		case SerialPacket::Type::REGION_REQUEST:
			deserializeRegionFormat(packet, reinterpret_cast<char*>(buffer));
		break;

		case SerialPacket::Type::REGION_CONTENT:
			deserializeRegionContent(packet, reinterpret_cast<char*>(buffer));
		break;

		//combat info
		case SerialPacket::Type::COMBAT_NEW:
		case SerialPacket::Type::COMBAT_DELETE:
		case SerialPacket::Type::COMBAT_UPDATE:

		//TODO: is this the best fit?
		case SerialPacket::Type::COMBAT_ENTER_REQUEST:
		case SerialPacket::Type::COMBAT_ENTER_RESPONSE:
		case SerialPacket::Type::COMBAT_EXIT_REQUEST:
		case SerialPacket::Type::COMBAT_EXIT_RESPONSE:

			serializeCombat(packet, reinterpret_cast<char*>(buffer));
		break;

		//character info
		case SerialPacket::Type::CHARACTER_NEW:
		case SerialPacket::Type::CHARACTER_DELETE:
		case SerialPacket::Type::CHARACTER_UPDATE:
		case SerialPacket::Type::CHARACTER_STATS_REQUEST:
		case SerialPacket::Type::CHARACTER_STATS_RESPONSE:
			deserializeCharacter(packet, reinterpret_cast<char*>(buffer));
		break;

		//enemy info
		case SerialPacket::Type::ENEMY_NEW:
		case SerialPacket::Type::ENEMY_DELETE:
		case SerialPacket::Type::ENEMY_UPDATE:
		case SerialPacket::Type::ENEMY_STATS_REQUEST:
		case SerialPacket::Type::ENEMY_STATS_RESPONSE:
			serializeEnemy(packet, reinterpret_cast<char*>(buffer));
		break;
	}
}