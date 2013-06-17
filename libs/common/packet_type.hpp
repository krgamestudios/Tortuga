#ifndef PACKETTYPE_HPP_
#define PACKETTYPE_HPP_

#include "vector2.hpp"

#include "SDL_net/SDL_net.h"

#define PACKET_STRING_SIZE 100

#pragma pack(push, 0)

enum class PacketType {
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
	PLAYER_MOVE = 11,
};

struct Metadata {
	PacketType type;
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
	char playerHandle[PACKET_STRING_SIZE];
	char playerAvatar[PACKET_STRING_SIZE];
	//TODO: player data
};

struct JoinResponse {
	Metadata meta;
	int playerIndex;
	//resource list
};

struct Disconnect {
	Metadata meta;
};

struct Synchronize {
	Metadata meta;
};

struct PlayerNew {
	Metadata meta;
	int playerIndex;
	//TODO Playerdata
};

struct PlayerDelete {
	Metadata meta;
	int playerIndex;
};

struct PlayerMove {
	Metadata meta;
	int playerIndex;
	Vector2 position;
	Vector2 motion;
};

union Packet {
	Packet() {
		meta.type = PacketType::NONE;
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

	PlayerNew playerNew;
	PlayerDelete playerDelete;
	PlayerMove playerMove;

#ifdef DEBUG
	char buffer[1024];
#endif
};

#pragma pack(pop)

#endif
