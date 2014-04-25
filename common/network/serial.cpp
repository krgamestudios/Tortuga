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
	SERIALIZE(buffer, &packet->clientInfo.playerIndex, sizeof(int));

	//texts
	SERIALIZE(buffer, packet->clientInfo.player, PACKET_STRING_SIZE);
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

void serializePlayer(SerialPacket* packet, char* buffer) {
	SERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//indexes
	SERIALIZE(buffer, &packet->playerInfo.clientIndex, sizeof(int));
	SERIALIZE(buffer, &packet->playerInfo.playerIndex, sizeof(int));

	//texts
	SERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	SERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);

	//vectors
	SERIALIZE(buffer, &packet->playerInfo.position.x, sizeof(double));
	SERIALIZE(buffer, &packet->playerInfo.position.y, sizeof(double));
	SERIALIZE(buffer, &packet->playerInfo.motion.x, sizeof(double));
	SERIALIZE(buffer, &packet->playerInfo.motion.y, sizeof(double));
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
	DESERIALIZE(buffer, &packet->clientInfo.playerIndex, sizeof(int));

	//texts
	DESERIALIZE(buffer, packet->clientInfo.player, PACKET_STRING_SIZE);
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

void deserializePlayer(SerialPacket* packet, char* buffer) {
	DESERIALIZE(buffer, &packet->meta.type, sizeof(SerialPacket::Type));

	//indexes
	DESERIALIZE(buffer, &packet->playerInfo.clientIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->playerInfo.playerIndex, sizeof(int));

	//texts
	DESERIALIZE(buffer, packet->clientInfo.handle, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, packet->clientInfo.avatar, PACKET_STRING_SIZE);

	//vectors
	DESERIALIZE(buffer, &packet->playerInfo.position.x, sizeof(double));
	DESERIALIZE(buffer, &packet->playerInfo.position.y, sizeof(double));
	DESERIALIZE(buffer, &packet->playerInfo.motion.x, sizeof(double));
	DESERIALIZE(buffer, &packet->playerInfo.motion.y, sizeof(double));
}

//-------------------------
//the interface functions
//-------------------------

void serialize(SerialPacket* packet, void* buffer) {
	switch(packet->meta.type) {
		//No extra data
		case SerialPacket::Type::NONE:
		case SerialPacket::Type::PING:
		case SerialPacket::Type::PONG:
		case SerialPacket::Type::BROADCAST_REQUEST:
			serializeType(packet, reinterpret_cast<char*>(buffer));
		break;

		//Server info
		case SerialPacket::Type::BROADCAST_RESPONSE:
			serializeServer(packet, reinterpret_cast<char*>(buffer));
		break;

		//Client info
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

		//Player info
		case SerialPacket::Type::PLAYER_NEW:
		case SerialPacket::Type::PLAYER_DELETE:
		case SerialPacket::Type::PLAYER_UPDATE:
			serializePlayer(packet, reinterpret_cast<char*>(buffer));
		break;
	}
}

void deserialize(SerialPacket* packet, void* buffer) {
	//find the type, so that you can actually deserialize the packet!
	deserializeType(packet, reinterpret_cast<char*>(buffer));
	switch(packet->meta.type) {
		//No extra data
		case SerialPacket::Type::NONE:
		case SerialPacket::Type::PING:
		case SerialPacket::Type::PONG:
		case SerialPacket::Type::BROADCAST_REQUEST:
			//NOTHING
		break;

		//Server info
		case SerialPacket::Type::BROADCAST_RESPONSE:
			deserializeServer(packet, reinterpret_cast<char*>(buffer));
		break;

		//Client info
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

		//Player info
		case SerialPacket::Type::PLAYER_NEW:
		case SerialPacket::Type::PLAYER_DELETE:
		case SerialPacket::Type::PLAYER_UPDATE:
			deserializePlayer(packet, reinterpret_cast<char*>(buffer));
		break;
	}
}