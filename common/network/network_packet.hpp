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
#ifndef NETWORKPACKET_HPP_
#define NETWORKPACKET_HPP_

#include "vector2.hpp"
#include "region.hpp"

#include "SDL/SDL_net.h"

#define PACKET_STRING_SIZE 100

#pragma pack(push, 0)

//TODO: rename to serial packet
union NetworkPacket {
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

		//disconnect from the server
		DISCONNECT = 7,

		//mass update
		SYNCHRONIZE = 8,

		//shut down the server
		SHUTDOWN = 9,

		//Player movement, etc.
		PLAYER_NEW = 10,
		PLAYER_DELETE = 11,
		PLAYER_UPDATE = 12,

		//map data
		REGION_REQUEST = 13,
		REGION_CONTENT = 14,
	};

	//metadata on the packet itself
	struct Metadata {
		Type type;
		IPaddress srcAddress;
	}meta;

	//information about the server
	struct ServerInformation {
		Metadata meta;
		//TODO: version info
		char name[PACKET_STRING_SIZE];
		//TODO: player count
		//TODO: map format
	}serverInfo;

	//information about the client
	//TODO: login credentials
	struct ClientInformation {
		Metadata meta;
		int index;
	}clientInfo;

	//information about a player
	struct PlayerInformation {
		Metadata meta;
		int clientIndex;
		int playerIndex;
		//TODO: should probably move these into the client info
		//TODO: these might actually do better during the login system
		char handle[PACKET_STRING_SIZE];
		char avatar[PACKET_STRING_SIZE];
		Vector2 position;
		Vector2 motion;
	}playerInfo;

	//map data
	struct RegionInformation {
		Metadata meta;
		int width, height, depth, x, y;
		Region* region;
	}regionInfo;

	//defaults
	NetworkPacket() {
		meta.type = Type::NONE;
		meta.srcAddress = {0,0};
	}
};

#pragma pack(pop)

#endif
