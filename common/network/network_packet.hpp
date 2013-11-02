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

#include "SDL/SDL_net.h"

#define PACKET_STRING_SIZE 100

#pragma pack(push, 0)

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

		//Player movement, etc.
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
	}serverInfo;
};

#pragma pack(pop)

#endif
