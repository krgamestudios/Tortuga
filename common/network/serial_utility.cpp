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
#include "serial_utility.hpp"

//packet types
#include "character_packet.hpp"
#include "client_packet.hpp"
#include "monster_packet.hpp"
#include "region_packet.hpp"
#include "server_packet.hpp"
#include "text_packet.hpp"

#include <cstring>

//macros
#define BOUNDS(type, lower, upper) ((type) > (lower) && (type) < (upper))

//raw memory copy
void serialCopy(void** buffer, void* data, int size) {
	memcpy(*buffer, data, size);
	*buffer = reinterpret_cast<char*>(*buffer) + size;
}

void deserialCopy(void** buffer, void* data, int size) {
	memcpy(data, *buffer, size);
	*buffer = reinterpret_cast<char*>(*buffer) + size;
}

//DOCS: The server and client MUST use the correct packet types

//main switch functions
void serializePacket(void* buffer, SerialPacketBase* packet) {
	if (BOUNDS(packet->type, SerialPacketType::FORMAT_SERVER, SerialPacketType::FORMAT_END_SERVER)) {
		serializeServer(buffer, static_cast<ServerPacket*>(packet));
	}

	if (BOUNDS(packet->type, SerialPacketType::FORMAT_CLIENT, SerialPacketType::FORMAT_END_CLIENT)) {
		serializeClient(buffer, static_cast<ClientPacket*>(packet));
	}

	if (BOUNDS(packet->type, SerialPacketType::FORMAT_REGION, SerialPacketType::FORMAT_END_REGION)) {
		serializeRegion(buffer, static_cast<RegionPacket*>(packet));
	}

	if (BOUNDS(packet->type, SerialPacketType::FORMAT_CHARACTER, SerialPacketType::FORMAT_END_CHARACTER)) {
		serializeCharacter(buffer, static_cast<CharacterPacket*>(packet));
	}

	if (BOUNDS(packet->type, SerialPacketType::FORMAT_MONSTER, SerialPacketType::FORMAT_END_MONSTER)) {
		serializeMonster(buffer, static_cast<MonsterPacket*>(packet));
	}

	if (BOUNDS(packet->type, SerialPacketType::FORMAT_TEXT, SerialPacketType::FORMAT_END_TEXT)) {
		serializeText(buffer, static_cast<TextPacket*>(packet));
	}
}

void deserializePacket(void* buffer, SerialPacketBase* packet) {
	//find the type, so that you can actually deserialize the packet!
	SerialPacketType type;
	memcpy(&type, buffer, sizeof(SerialPacketType));

	if (BOUNDS(type, SerialPacketType::FORMAT_SERVER, SerialPacketType::FORMAT_END_SERVER)) {
		deserializeServer(buffer, static_cast<ServerPacket*>(packet));
	}

	if (BOUNDS(type, SerialPacketType::FORMAT_CLIENT, SerialPacketType::FORMAT_END_CLIENT)) {
		deserializeClient(buffer, static_cast<ClientPacket*>(packet));
	}

	if (BOUNDS(type, SerialPacketType::FORMAT_REGION, SerialPacketType::FORMAT_END_REGION)) {
		deserializeRegion(buffer, static_cast<RegionPacket*>(packet));
	}

	if (BOUNDS(type, SerialPacketType::FORMAT_CHARACTER, SerialPacketType::FORMAT_END_CHARACTER)) {
		deserializeCharacter(buffer, static_cast<CharacterPacket*>(packet));
	}

	if (BOUNDS(type, SerialPacketType::FORMAT_MONSTER, SerialPacketType::FORMAT_END_MONSTER)) {
		deserializeMonster(buffer, static_cast<MonsterPacket*>(packet));
	}

	if (BOUNDS(type, SerialPacketType::FORMAT_TEXT, SerialPacketType::FORMAT_END_TEXT)) {
		deserializeText(buffer, static_cast<TextPacket*>(packet));
	}
}