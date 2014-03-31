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

#include "map_generator.hpp"

#include <cstring>

//-------------------------
//internal serialization functions
//-------------------------

void serializeType(NetworkPacket* packet, char* buffer) {
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
}

void serializeServer(NetworkPacket* packet, char* buffer) {
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);
	memcpy(buffer, packet->serverInfo.name, PACKET_STRING_SIZE);
}

void serializeClient(NetworkPacket* packet, char* buffer) {
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);
	memcpy(buffer, &packet->clientInfo.index, sizeof(int));
}

void serializePlayer(NetworkPacket* packet, char* buffer) {
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);

	//indexes
	memcpy(buffer, &packet->playerInfo.clientIndex, sizeof(int));
	buffer += sizeof(int);
	memcpy(buffer, &packet->playerInfo.playerIndex, sizeof(int));
	buffer += sizeof(int);

	//text
	memcpy(buffer, packet->playerInfo.handle, PACKET_STRING_SIZE);
	buffer += PACKET_STRING_SIZE;
	memcpy(buffer, packet->playerInfo.avatar, PACKET_STRING_SIZE);
	buffer += PACKET_STRING_SIZE;

	//vectors
	memcpy(buffer, &packet->playerInfo.position.x, sizeof(double));
	buffer += sizeof(double);
	memcpy(buffer, &packet->playerInfo.position.y, sizeof(double));
	buffer += sizeof(double);
	memcpy(buffer, &packet->playerInfo.motion.x, sizeof(double));
	buffer += sizeof(double);
	memcpy(buffer, &packet->playerInfo.motion.y, sizeof(double));
}

void serializeRegionFormat(NetworkPacket* packet, char* buffer) {
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);

	//size
	memcpy(buffer, &packet->regionInfo.width, sizeof(int));
	buffer += sizeof(int);
	memcpy(buffer, &packet->regionInfo.height, sizeof(int));
	buffer += sizeof(int);
	memcpy(buffer, &packet->regionInfo.depth, sizeof(int));
	buffer += sizeof(int);

	//x & y
	memcpy(buffer, &packet->regionInfo.x, sizeof(int));
	buffer += sizeof(int);
	memcpy(buffer, &packet->regionInfo.y, sizeof(int));
}

void serializeRegionContent(NetworkPacket* packet, char* buffer) {
	//format
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);

	//size
	*reinterpret_cast<int*>(buffer) = packet->regionInfo.region->GetWidth();
	buffer += sizeof(int);
	*reinterpret_cast<int*>(buffer) = packet->regionInfo.region->GetHeight();
	buffer += sizeof(int);
	*reinterpret_cast<int*>(buffer) = packet->regionInfo.region->GetDepth();
	buffer += sizeof(int);

	//x & y
	*reinterpret_cast<int*>(buffer) = packet->regionInfo.region->GetX();
	buffer += sizeof(int);
	*reinterpret_cast<int*>(buffer) = packet->regionInfo.region->GetY();
	buffer += sizeof(int);

	//content
	for (register int i = 0; i < packet->regionInfo.region->GetWidth(); i++) {
		for (register int j = 0; j < packet->regionInfo.region->GetHeight(); j++) {
			for (register int k = 0; k < packet->regionInfo.region->GetDepth(); k++) {
				*reinterpret_cast<Region::type_t*>(buffer) = packet->regionInfo.region->GetTile(i, j, k);
				buffer += sizeof(Region::type_t);
			}
		}
	}
}

//-------------------------
//internal deserialization functions
//-------------------------

void deserializeType(NetworkPacket* packet, char* buffer) {
	memcpy(&packet->meta.type, buffer, sizeof(NetworkPacket::Type));
}

void deserializeServer(NetworkPacket* packet, char* buffer) {
	memcpy(&packet->meta.type, buffer, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);
	memcpy(packet->serverInfo.name, buffer, PACKET_STRING_SIZE);
}

void deserializeClient(NetworkPacket* packet, char* buffer) {
	memcpy(&packet->meta.type, buffer, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);
	memcpy(&packet->clientInfo.index, buffer, sizeof(int));
}

void deserializePlayer(NetworkPacket* packet, char* buffer) {
	memcpy(&packet->meta.type, buffer, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);

	//indexes
	memcpy(&packet->playerInfo.clientIndex, buffer, sizeof(int));
	buffer += sizeof(int);
	memcpy(&packet->playerInfo.playerIndex, buffer, sizeof(int));
	buffer += sizeof(int);

	//text
	memcpy(packet->playerInfo.handle, buffer, PACKET_STRING_SIZE);
	buffer += PACKET_STRING_SIZE;
	memcpy(packet->playerInfo.avatar, buffer, PACKET_STRING_SIZE);
	buffer += PACKET_STRING_SIZE;

	//vectors
	memcpy(&packet->playerInfo.position.x, buffer, sizeof(double));
	buffer += sizeof(double);
	memcpy(&packet->playerInfo.position.y, buffer, sizeof(double));
	buffer += sizeof(double);
	memcpy(&packet->playerInfo.motion.x, buffer, sizeof(double));
	buffer += sizeof(double);
	memcpy(&packet->playerInfo.motion.y, buffer, sizeof(double));
}

void deserializeRegionFormat(NetworkPacket* packet, char* buffer) {
	memcpy(&packet->meta.type, buffer, sizeof(NetworkPacket::Type));
	buffer += sizeof(NetworkPacket::Type);

	//size
	memcpy(&packet->regionInfo.width, buffer, sizeof(int));
	buffer += sizeof(int);
	memcpy(&packet->regionInfo.height, buffer, sizeof(int));
	buffer += sizeof(int);
	memcpy(&packet->regionInfo.depth, buffer, sizeof(int));
	buffer += sizeof(int);

	//x & y
	memcpy(&packet->regionInfo.x, buffer, sizeof(int));
	buffer += sizeof(int);
	memcpy(&packet->regionInfo.y, buffer, sizeof(int));
}

void deserializeRegionContent(NetworkPacket* packet, char* buffer) {
	//format
	deserializeRegionFormat(packet, buffer);
	buffer += sizeof(int) * 5 + sizeof(NetworkPacket::Type);

	//content
	BlankGenerator().Create(
		&packet->regionInfo.region,
		packet->regionInfo.width,
		packet->regionInfo.height,
		packet->regionInfo.depth,
		packet->regionInfo.x,
		packet->regionInfo.y
	);

	for (register int i = 0; i < packet->regionInfo.region->GetWidth(); i++) {
		for (register int j = 0; j < packet->regionInfo.region->GetHeight(); j++) {
			for (register int k = 0; k < packet->regionInfo.region->GetDepth(); k++) {
				packet->regionInfo.region->SetTile(i, j, k, *reinterpret_cast<Region::type_t*>(buffer));
				buffer += sizeof(Region::type_t);
			}
		}
	}
}

//-------------------------
//the interface functions
//-------------------------

void serialize(NetworkPacket* packet, void* buffer) {
	switch(packet->meta.type) {
		//No extra data
		case NetworkPacket::Type::NONE:
		case NetworkPacket::Type::PING:
		case NetworkPacket::Type::PONG:
		case NetworkPacket::Type::BROADCAST_REQUEST:
		case NetworkPacket::Type::JOIN_REQUEST:
		case NetworkPacket::Type::SYNCHRONIZE:
			serializeType(packet, reinterpret_cast<char*>(buffer));
		break;

		//Server info
		case NetworkPacket::Type::BROADCAST_RESPONSE:
			serializeServer(packet, reinterpret_cast<char*>(buffer));
		break;

		//Client info
		case NetworkPacket::Type::JOIN_RESPONSE:
		case NetworkPacket::Type::DISCONNECT:
		case NetworkPacket::Type::SHUTDOWN:
			serializeClient(packet, reinterpret_cast<char*>(buffer));
		break;

		//Player info
		case NetworkPacket::Type::PLAYER_NEW:
		case NetworkPacket::Type::PLAYER_DELETE:
		case NetworkPacket::Type::PLAYER_UPDATE:
			serializePlayer(packet, reinterpret_cast<char*>(buffer));
		break;

		//region info
		case NetworkPacket::Type::REGION_REQUEST:
			serializeRegionFormat(packet, reinterpret_cast<char*>(buffer));
		break;

		case NetworkPacket::Type::REGION_CONTENT:
			serializeRegionContent(packet, reinterpret_cast<char*>(buffer));
		break;
	}
}

void deserialize(NetworkPacket* packet, void* buffer) {
	//find the type, so that you can actually deserialize the packet!
	deserializeType(packet, reinterpret_cast<char*>(buffer));
	switch(packet->meta.type) {
		//No extra data
		case NetworkPacket::Type::NONE:
		case NetworkPacket::Type::PING:
		case NetworkPacket::Type::PONG:
		case NetworkPacket::Type::BROADCAST_REQUEST:
		case NetworkPacket::Type::JOIN_REQUEST:
		case NetworkPacket::Type::SYNCHRONIZE:
			//NOTHING
		break;

		//Server info
		case NetworkPacket::Type::BROADCAST_RESPONSE:
			deserializeServer(packet, reinterpret_cast<char*>(buffer));
		break;

		//Client info
		case NetworkPacket::Type::JOIN_RESPONSE:
		case NetworkPacket::Type::DISCONNECT:
		case NetworkPacket::Type::SHUTDOWN:
			deserializeClient(packet, reinterpret_cast<char*>(buffer));
		break;

		//Player info
		case NetworkPacket::Type::PLAYER_NEW:
		case NetworkPacket::Type::PLAYER_DELETE:
		case NetworkPacket::Type::PLAYER_UPDATE:
			deserializePlayer(packet, reinterpret_cast<char*>(buffer));
		break;

		//region info
		case NetworkPacket::Type::REGION_REQUEST:
			deserializeRegionFormat(packet, reinterpret_cast<char*>(buffer));
		break;

		case NetworkPacket::Type::REGION_CONTENT:
			deserializeRegionContent(packet, reinterpret_cast<char*>(buffer));
		break;
	}
}