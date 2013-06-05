#ifndef PACKETLIST_HPP_
#define PACKETLIST_HPP_ 

#include "vector2.hpp"

#include <string>

#define PACKET_STRING_SIZE 100

enum class PacketList {
	NONE = 0,

	//connections
	PING = 1,
	PONG = 2,
	JOINREQUEST = 3,
	JOINCONFIRM = 4,
	DISCONNECT = 5,

	//information control
	SYNCHRONIZE = 6,
	NEWPLAYER = 7,
	DELETEPLAYER = 8,
	MOVEMENT = 9,
};

//-------------------------
//connections
//-------------------------

struct Ping {
	PacketList type = PacketList::PING;
	char metadata[PACKET_STRING_SIZE];
};

struct Pong {
	PacketList type = PacketList::PONG;
	char metadata[PACKET_STRING_SIZE];
};

struct JoinRequest {
	PacketList type = PacketList::JOINREQUEST;
	char handle[PACKET_STRING_SIZE];
	char avatar[PACKET_STRING_SIZE];
};

struct JoinConfirm {
	PacketList type = PacketList::JOINCONFIRM;
	int playerID;
};

struct Disconnect {
	PacketList type = PacketList::DISCONNECT;
	int playerID;
};

//-------------------------
//information control
//-------------------------

struct Synchronize {
	PacketList type = PacketList::SYNCHRONIZE;
	int playerID;
};

struct NewPlayer {
	PacketList type = PacketList::NEWPLAYER;
	int playerID;
	char handle[PACKET_STRING_SIZE];
	char avatar[PACKET_STRING_SIZE];
	Vector2 position;
	Vector2 motion;
};

struct DeletePlayer {
	PacketList type = PacketList::DELETEPLAYER;
	int playerID;
};

struct Movement {
	PacketList type = PacketList::MOVEMENT;
	int playerID;
	Vector2 position;
	Vector2 motion;
};

//-------------------------
//the packet itself
//-------------------------

union PacketData {
	PacketData() {};
	PacketList type = PacketList::NONE;

	//connections
	Ping ping;
	Pong pong;
	JoinRequest joinRequest;
	JoinConfirm joinConfirm;
	Disconnect disconnect;

	//information control
	Synchronize synchronize;
	NewPlayer newPlayer;
	DeletePlayer deletePlayer;
	Movement movement;
};

#endif
