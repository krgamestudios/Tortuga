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
#include "serial_utility.hpp"

#include "serial_packet_type.hpp"

#include "server_packet.hpp"
#include "client_packet.hpp"
#include "region_packet.hpp"
#include "character_packet.hpp"

#include <cstring>

void serializePacket(SerialPacketBase* packet, void* data) {
	switch(packet->GetType()) {
		case SerialPacketType::PING:
		case SerialPacketType::PONG:
		case SerialPacketType::BROADCAST_REQUEST:
		case SerialPacketType::BROADCAST_RESPONSE:
			static_cast<ServerPacket*>(packet)->Serialize(data);
		break;
		case SerialPacketType::JOIN_REQUEST:
		case SerialPacketType::JOIN_RESPONSE:
		case SerialPacketType::JOIN_REJECTION:
		case SerialPacketType::SYNCHRONIZE:
		case SerialPacketType::DISCONNECT:
		case SerialPacketType::SHUTDOWN:
			static_cast<ClientPacket*>(packet)->Serialize(data);
		break;
		case SerialPacketType::REGION_REQUEST:
		case SerialPacketType::REGION_CONTENT:
			static_cast<RegionPacket*>(packet)->Serialize(data);
		break;
		case SerialPacketType::CHARACTER_NEW:
		case SerialPacketType::CHARACTER_DELETE:
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
		case SerialPacketType::CHARACTER_STATS_RESPONSE:
		case SerialPacketType::CHARACTER_REJECTION:
			static_cast<CharacterPacket*>(packet)->Serialize(data);
		break;
	}
}

void deserializePacket(SerialPacketBase* packet, void* data) {
	//get the type
	SerialPacketType type;
	memcpy(&type, data, sizeof(SerialPacketType));

	switch(type) {
		case SerialPacketType::PING:
		case SerialPacketType::PONG:
		case SerialPacketType::BROADCAST_REQUEST:
		case SerialPacketType::BROADCAST_RESPONSE:
			static_cast<ServerPacket*>(packet)->Deserialize(data);
		break;
		case SerialPacketType::JOIN_REQUEST:
		case SerialPacketType::JOIN_RESPONSE:
		case SerialPacketType::JOIN_REJECTION:
		case SerialPacketType::SYNCHRONIZE:
		case SerialPacketType::DISCONNECT:
		case SerialPacketType::SHUTDOWN:
			static_cast<ClientPacket*>(packet)->Deserialize(data);
		break;
		case SerialPacketType::REGION_REQUEST:
		case SerialPacketType::REGION_CONTENT:
			static_cast<RegionPacket*>(packet)->Deserialize(data);
		break;
		case SerialPacketType::CHARACTER_NEW:
		case SerialPacketType::CHARACTER_DELETE:
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
		case SerialPacketType::CHARACTER_STATS_RESPONSE:
		case SerialPacketType::CHARACTER_REJECTION:
			static_cast<CharacterPacket*>(packet)->Deserialize(data);
		break;
	}
}

void serializeCopy(void** bufferHead, void* data, int size) {
	memcpy(*bufferHead, data, size);
	(*bufferHead) = static_cast<void*>(static_cast<char*>(*bufferHead) + size);
}

void deserializeCopy(void** bufferHead, void* data, int size) {
	memcpy(data, *bufferHead, size);
	(*bufferHead) = static_cast<void*>(static_cast<char*>(*bufferHead) + size);
}

void serializeCopyStatistics(void** bufferHead, Statistics* stats) {
	//integers
	serializeCopy(bufferHead, &stats->level, sizeof(int));
	serializeCopy(bufferHead, &stats->exp, sizeof(int));
	serializeCopy(bufferHead, &stats->maxHP, sizeof(int));
	serializeCopy(bufferHead, &stats->health, sizeof(int));
	serializeCopy(bufferHead, &stats->maxMP, sizeof(int));
	serializeCopy(bufferHead, &stats->mana, sizeof(int));
	serializeCopy(bufferHead, &stats->attack, sizeof(int));
	serializeCopy(bufferHead, &stats->defence, sizeof(int));
	serializeCopy(bufferHead, &stats->intelligence, sizeof(int));
	serializeCopy(bufferHead, &stats->resistance, sizeof(int));
	serializeCopy(bufferHead, &stats->speed, sizeof(int));

	//floats
	serializeCopy(bufferHead, &stats->accuracy, sizeof(float));
	serializeCopy(bufferHead, &stats->evasion, sizeof(float));
	serializeCopy(bufferHead, &stats->luck, sizeof(float));
}

void deserializeCopyStatistics(void** bufferHead, Statistics* stats) {
	//integers
	deserializeCopy(bufferHead, &stats->level, sizeof(int));
	deserializeCopy(bufferHead, &stats->exp, sizeof(int));
	deserializeCopy(bufferHead, &stats->maxHP, sizeof(int));
	deserializeCopy(bufferHead, &stats->health, sizeof(int));
	deserializeCopy(bufferHead, &stats->maxMP, sizeof(int));
	deserializeCopy(bufferHead, &stats->mana, sizeof(int));
	deserializeCopy(bufferHead, &stats->attack, sizeof(int));
	deserializeCopy(bufferHead, &stats->defence, sizeof(int));
	deserializeCopy(bufferHead, &stats->intelligence, sizeof(int));
	deserializeCopy(bufferHead, &stats->resistance, sizeof(int));
	deserializeCopy(bufferHead, &stats->speed, sizeof(int));

	//floats
	deserializeCopy(bufferHead, &stats->accuracy, sizeof(float));
	deserializeCopy(bufferHead, &stats->evasion, sizeof(float));
	deserializeCopy(bufferHead, &stats->luck, sizeof(float));
}
