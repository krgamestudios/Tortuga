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

#include <cstring>
#include <iostream>
using namespace std;

//-------------------------
//internal serialization functions
//-------------------------

void serializeType(NetworkPacket* packet, void* buffer) {
	memcpy(buffer, &packet->meta.type, sizeof(NetworkPacket::Type));
}

void serializeServer(NetworkPacket* packet, void* buffer) {
	int len = 0;
	memcpy((void*)((int)buffer + len), &packet->meta.type, sizeof(NetworkPacket::Type));
	len += sizeof(NetworkPacket::Type);
	memcpy((void*)((int)buffer + len), packet->serverInfo.name, PACKET_STRING_SIZE);
	len += PACKET_STRING_SIZE;
}

void serializeClient(NetworkPacket* packet, void* buffer) {
	//TODO
}

void serializePlayer(NetworkPacket* packet, void* buffer) {
	//TODO
}

//-------------------------
//internal deserialization functions
//-------------------------

void deserializeType(NetworkPacket* packet, void* buffer) {
	memcpy(&packet->meta.type, buffer, sizeof(NetworkPacket::Type));
}

void deserializeServer(NetworkPacket* packet, void* buffer) {
	int len = 0;
	memcpy(&packet->meta.type, (void*)((int)buffer + len), sizeof(NetworkPacket::Type));
	len += sizeof(NetworkPacket::Type);
	memcpy(packet->serverInfo.name, (void*)((int)buffer + len), PACKET_STRING_SIZE);
	len += PACKET_STRING_SIZE;
}

void deserializeClient(NetworkPacket* packet, void* buffer) {
	//TODO
}

void deserializePlayer(NetworkPacket* packet, void* buffer) {
	//TODO
}

//-------------------------
//the interface functions
//-------------------------

void serialize(NetworkPacket* packet, void* buffer) {
	cout << (int)packet->meta.type << endl;
	switch(packet->meta.type) {
		//No extra data
		case NetworkPacket::Type::NONE:
		case NetworkPacket::Type::PING:
		case NetworkPacket::Type::PONG:
		case NetworkPacket::Type::BROADCAST_REQUEST:
		case NetworkPacket::Type::JOIN_REQUEST:
		case NetworkPacket::Type::SHUTDOWN:
			serializeType(packet, buffer);
		break;

		//Server info
		case NetworkPacket::Type::BROADCAST_RESPONSE:
			serializeServer(packet, buffer);
		break;

		//Client info
		case NetworkPacket::Type::JOIN_RESPONSE:
		case NetworkPacket::Type::DISCONNECT:
		case NetworkPacket::Type::SYNCHRONIZE:
			serializeClient(packet, buffer);
		break;

		//Player info
		case NetworkPacket::Type::PLAYER_NEW:
		case NetworkPacket::Type::PLAYER_DELETE:
		case NetworkPacket::Type::PLAYER_UPDATE:
			serializePlayer(packet, buffer);
		break;
	}
}

void deserialize(NetworkPacket* packet, void* buffer) {
	cout << (int)packet->meta.type << endl;
	switch(packet->meta.type) {
		//No extra data
		case NetworkPacket::Type::NONE:
		case NetworkPacket::Type::PING:
		case NetworkPacket::Type::PONG:
		case NetworkPacket::Type::BROADCAST_REQUEST:
		case NetworkPacket::Type::JOIN_REQUEST:
		case NetworkPacket::Type::SHUTDOWN:
			deserializeType(packet, buffer);
		break;

		//Server info
		case NetworkPacket::Type::BROADCAST_RESPONSE:
			deserializeServer(packet, buffer);
		break;

		//Client info
		case NetworkPacket::Type::JOIN_RESPONSE:
		case NetworkPacket::Type::DISCONNECT:
		case NetworkPacket::Type::SYNCHRONIZE:
			deserializeClient(packet, buffer);
		break;

		//Player info
		case NetworkPacket::Type::PLAYER_NEW:
		case NetworkPacket::Type::PLAYER_DELETE:
		case NetworkPacket::Type::PLAYER_UPDATE:
			deserializePlayer(packet, buffer);
		break;
	}
}