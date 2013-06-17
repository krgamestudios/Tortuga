#ifndef PACKETTYPE_HPP_
#define PACKETTYPE_HPP_

#include "vector2.hpp"

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

struct Ping {
	PacketType type;
};

struct Pong {
	PacketType type;
};

struct BroadcastRequest {
	PacketType type;
};

struct BroadcastResponse {
	PacketType type;
	char name[PACKET_STRING_SIZE];
	//TODO: version
};

struct JoinRequest {
	PacketType type;
	char playerHandle[PACKET_STRING_SIZE];
	char playerAvatar[PACKET_STRING_SIZE];
	//TODO: player data
};

struct JoinResponse {
	PacketType type;
	int playerIndex;
	//resource list
};

struct Disconnect {
	PacketType type;
};

struct Synchronize {
	PacketType type;
};

struct PlayerNew {
	PacketType type;
	int playerIndex;
	//TODO Playerdata
};

struct PlayerDelete {
	PacketType type;
	int playerIndex;
};

struct PlayerMove {
	PacketType type;
	int playerIndex;
	Vector2 position;
	Vector2 motion;
};

union Packet {
	Packet() { type = PacketType::NONE; };
	PacketType type;

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
};

#pragma pack(pop)

#endif
