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
	PacketType type = PacketType::PING;
};

struct Pong {
	PacketType type = PacketType::PONG;
};

struct BroadcastRequest {
	PacketType type = PacketType::BROADCAST_REQUEST;
};

struct BroadcastResponse {
	PacketType type = PacketType::BROADCAST_RESPONSE;
	char name[PACKET_STRING_SIZE];
	//TODO: version
};

struct JoinRequest {
	PacketType type = PacketType::JOIN_REQUEST;
	//TODO: player data
};

struct JoinResponse {
	PacketType type = PacketType::JOIN_RESPONSE;
	int playerIndex;
	//resource list
};

struct Disconnect {
	PacketType type = PacketType::DISCONNECT;
};

struct Synchronize {
	PacketType type = PacketType::SYNCHRONIZE;
};

struct PlayerNew {
	PacketType type = PacketType::PLAYER_NEW;
	int playerIndex;
	//TODO Playerdata
};

struct PlayerDelete {
	PacketType type = PacketType::PLAYER_DELETE;
	int playerIndex;
};

struct PlayerMove {
	PacketType type = PacketType::PLAYER_MOVE;
	int playerIndex;
	Vector2 position;
	Vector2 motion;
};

union Packet {
	Packet() {}
	PacketType type = PacketType::NONE;

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
