/* Copyright: (c) Kayne Ruse 2013
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
#ifndef PACKETTYPE_HPP_
#define PACKETTYPE_HPP_

#include "vector2.hpp"

#include "SDL_net/SDL_net.h"

#define PACKET_STRING_SIZE 100

#pragma pack(push, 0)

namespace Packet {

enum class Type {
	NONE = 0,

	PING = 1,
	PONG = 2,
	BROADCAST_REQUEST = 3,
	BROADCAST_RESPONSE = 4,
	JOIN_REQUEST = 5,
	JOIN_RESPONSE = 6,
	DISCONNECT = 7,

	SYNCHRONIZE = 8,

	PLAYER_NEW = 9,
	PLAYER_DELETE = 10,
	PLAYER_UPDATE = 11,
};

struct Metadata {
	Type type;
	IPaddress address;
};

struct Ping {
	Metadata meta;
};

struct Pong {
	Metadata meta;
};

struct BroadcastRequest {
	Metadata meta;
};

struct BroadcastResponse {
	Metadata meta;
	char name[PACKET_STRING_SIZE];
	//TODO: version
};

struct JoinRequest {
	Metadata meta;
};

struct JoinResponse {
	Metadata meta;
	int clientIndex;
	//resource list
};

struct Disconnect {
	Metadata meta;
	int clientIndex;
};

struct Synchronize {
	Metadata meta;
};

struct PlayerNew {
	Metadata meta;
	int playerIndex;
	int clientIndex;
	char handle[PACKET_STRING_SIZE];
	char avatar[PACKET_STRING_SIZE];
	Vector2 position;
	Vector2 motion;
	//TODO Playerdata
};

struct PlayerDelete {
	Metadata meta;
	int playerIndex;
	int clientIndex;
};

struct PlayerUpdate {
	Metadata meta;
	int playerIndex;
	int clientIndex;
	Vector2 position;
	Vector2 motion;
};

union Packet {
	Packet() {
		meta.type = Type::NONE;
		meta.address.host = 0;
		meta.address.port = 0;
	};
	Metadata meta;

	Ping ping;
	Pong pong;
	BroadcastRequest broadcastRequest;
	BroadcastResponse broadcastResponse;
	JoinRequest joinRequest;
	JoinResponse joinResponse;
	Disconnect disconnect;

	Synchronize sync;

	PlayerNew playerNew;
	PlayerDelete playerDelete;
	PlayerUpdate playerUpdate;

#ifdef DEBUG
	char buffer[1024];
#endif
};

} //namespace Packet

#pragma pack(pop)

#endif
