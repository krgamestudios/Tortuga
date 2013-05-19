#ifndef PACKETLIST_HPP_
#define PACKETLIST_HPP_ 

#include "vector2.hpp"

#include <string>

#define PACKET_STRING_SIZE 256

enum class PacketList {
	//networking systems
	PING, PONG,
	JOINREQUEST,
	JOINCONFIRM,

	//connections
	NEWPLAYER,
	DELETEPLAYER,

	//play updates
	MOTIONUPDATE,
};

//-------------------------
//networking systems
//-------------------------

struct Ping {
	PacketList type = PacketList::PING;
	char buffer[PACKET_STRING_SIZE];
};

struct Pong {
	PacketList type = PacketList::PONG;
	char buffer[PACKET_STRING_SIZE];
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

//-------------------------
//connections
//-------------------------

struct NewPlayer {
	PacketList type = PacketList::NEWPLAYER;
	int playerID;
	char handle[PACKET_STRING_SIZE];
	char avatar[PACKET_STRING_SIZE];
};

struct DeletePlayer {
	PacketList type = PacketList::DELETEPLAYER;
	int playerID;
};

//-------------------------
//play updates
//-------------------------

struct MotionUpdate {
	PacketList type = PacketList::MOTIONUPDATE;
	int playerID;
	Vector2 position;
	Vector2 motion;
};

//-------------------------
//this state of this is great
//-------------------------

union Packet {
	Packet() {};
	~Packet() {};
	PacketList type;
	//networking systems
	Ping ping;
	Pong pong;
	JoinRequest joinRequest;
	JoinConfirm joinConfirm;
	//connections
	NewPlayer newPlayer;
	DeletePlayer deletePlayer;
	//play updates
	MotionUpdate motionUpdate;
};

#endif
