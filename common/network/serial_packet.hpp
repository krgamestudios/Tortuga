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
#ifndef SERIALPACKET_HPP_
#define SERIALPACKET_HPP_

#include "vector2.hpp"
#include "region.hpp"

#include "SDL/SDL_net.h"

#define NETWORK_VERSION 20140426
#define PACKET_STRING_SIZE 100

#pragma pack(push, 0)

union SerialPacket {
	//types of packets
	enum class Type {
		//default: there is something wrong
		NONE = 0,

		//not used
		PING = 1,
		PONG = 2,

		//Searching for a server to join
		BROADCAST_REQUEST = 3,
		BROADCAST_RESPONSE = 4,

		//try to join the server
		JOIN_REQUEST = 5,
		JOIN_RESPONSE = 6,

		//mass update
		SYNCHRONIZE = 7,

		//disconnect from the server
		DISCONNECT = 8,

		//shut down the server
		SHUTDOWN = 9,

		//map data
		REGION_REQUEST = 10,
		REGION_CONTENT = 11,

		//Player movement, etc.
		PLAYER_NEW = 12,
		PLAYER_DELETE = 13,
		PLAYER_UPDATE = 14,

		//TODO: combat packets
	};

	//metadata on the packet itself
	struct Metadata {
		Type type;
		IPaddress srcAddress;
	}meta;

	//information about the server
	struct ServerInformation {
		Metadata meta;
		int networkVersion;
		char name[PACKET_STRING_SIZE];
		int playerCount;
	}serverInfo;

	//information about the client
	struct ClientInformation {
		Metadata meta;
		int clientIndex;
		int playerIndex;
		char player[PACKET_STRING_SIZE];
		char handle[PACKET_STRING_SIZE];
		char avatar[PACKET_STRING_SIZE];
	}clientInfo;

	//map data
	struct RegionInformation {
		Metadata meta;
		int mapIndex;
		int x, y;
		Region* region;
	}regionInfo;

	//information about a player
	struct PlayerInformation {
		Metadata meta;
		int clientIndex;
		int playerIndex;
		Vector2 position;
		Vector2 motion;
	}playerInfo;

	//defaults
	SerialPacket() {
		meta.type = Type::NONE;
		meta.srcAddress = {0,0};
	}
};

#pragma pack(pop)

#endif
