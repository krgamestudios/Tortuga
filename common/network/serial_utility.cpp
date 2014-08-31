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

//packet types
#include "character_packet.hpp"
#include "client_packet.hpp"
#include "region_packet.hpp"
#include "server_packet.hpp"

#include <cstring>

//raw memory copy
void serialCopy(void** buffer, void* data, int size) {
	memcpy(*buffer, data, size);
	*buffer = reinterpret_cast<char*>(*buffer) + size;
}

void deserialCopy(void** buffer, void* data, int size) {
	memcpy(data, *buffer, size);
	*buffer = reinterpret_cast<char*>(*buffer) + size;
}

//main switch functions
void serializePacket(void* buffer, SerialPacketBase* packet) {
	switch(packet->type) {
		case SerialPacketType::PING:
		case SerialPacketType::PONG:
		case SerialPacketType::BROADCAST_REQUEST:
		case SerialPacketType::BROADCAST_RESPONSE:
			serializeServer(buffer, static_cast<ServerPacket*>(packet));
		break;
		case SerialPacketType::JOIN_REQUEST:
		case SerialPacketType::JOIN_RESPONSE:
		case SerialPacketType::JOIN_REJECTION:
		case SerialPacketType::SYNCHRONIZE:
		case SerialPacketType::DISCONNECT:
		case SerialPacketType::SHUTDOWN:
			serializeClient(buffer, static_cast<ClientPacket*>(packet));
		break;
		case SerialPacketType::REGION_REQUEST:
		case SerialPacketType::REGION_CONTENT:
			serializeRegion(buffer, static_cast<RegionPacket*>(packet));
		break;
		case SerialPacketType::CHARACTER_NEW:
		case SerialPacketType::CHARACTER_DELETE:
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
		case SerialPacketType::CHARACTER_STATS_RESPONSE:
		case SerialPacketType::CHARACTER_REJECTION:
			serializeCharacter(buffer, static_cast<CharacterPacket*>(packet));
		break;
	}
}

void deserializePacket(void* buffer, SerialPacketBase* packet) {
	//find the type, so that you can actually deserialize the packet!
	SerialPacketType type;
	memcpy(&type, buffer, sizeof(SerialPacketType));

	switch(type) {
		case SerialPacketType::PING:
		case SerialPacketType::PONG:
		case SerialPacketType::BROADCAST_REQUEST:
		case SerialPacketType::BROADCAST_RESPONSE:
			deserializeServer(buffer, static_cast<ServerPacket*>(packet));
		break;
		case SerialPacketType::JOIN_REQUEST:
		case SerialPacketType::JOIN_RESPONSE:
		case SerialPacketType::JOIN_REJECTION:
		case SerialPacketType::SYNCHRONIZE:
		case SerialPacketType::DISCONNECT:
		case SerialPacketType::SHUTDOWN:
			deserializeClient(buffer, static_cast<ClientPacket*>(packet));
		break;
		case SerialPacketType::REGION_REQUEST:
		case SerialPacketType::REGION_CONTENT:
			deserializeRegion(buffer, static_cast<RegionPacket*>(packet));
		break;
		case SerialPacketType::CHARACTER_NEW:
		case SerialPacketType::CHARACTER_DELETE:
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
		case SerialPacketType::CHARACTER_STATS_RESPONSE:
		case SerialPacketType::CHARACTER_REJECTION:
			deserializeCharacter(buffer, static_cast<CharacterPacket*>(packet));
		break;
	}
}