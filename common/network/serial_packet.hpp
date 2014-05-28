/* Copyright: (c) Kayne Ruse 2013, 2014
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
#include "statistics.hpp"

#include "SDL/SDL_net.h"

#define NETWORK_VERSION 20140528
#define PACKET_STRING_SIZE 100

union SerialPacket {
	//types of packets
	enum class Type {
		//default: there is something wrong
		NONE = 0,

		//keep alive
		PING = 1,
		PONG = 2,

		//searching for a server to join
		BROADCAST_REQUEST = 3,
		BROADCAST_RESPONSE = 4,
		BROADCAST_REJECTION = 5,

		//try to join the server
		JOIN_REQUEST = 6,
		JOIN_RESPONSE = 7,
		JOIN_REJECTION = 8,

		//mass update
		SYNCHRONIZE = 9,

		//disconnect from the server
		DISCONNECT = 10,

		//shut down the server
		SHUTDOWN = 11,

		//map data
		REGION_REQUEST = 12,
		REGION_CONTENT = 13,
		REGION_REJECTION = 14,

		//combat data
		COMBAT_ENTER = 15,
		COMBAT_EXIT = 16,

		COMBAT_UPDATE = 17,

		COMBAT_REJECTION = 18,

		//character data
		CHARACTER_NEW = 19,
		CHARACTER_DELETE = 20,
		CHARACTER_UPDATE = 21,

		CHARACTER_STATS_REQUEST = 22,
		CHARACTER_STATS_RESPONSE = 23,

		CHARACTER_REJECTION = 24,

		//enemy data
		ENEMY_NEW = 25,
		ENEMY_DELETE = 26,
		ENEMY_UPDATE = 27,

		ENEMY_STATS_REQUEST = 28,
		ENEMY_STATS_RESPONSE = 29,

		ENEMY_REJECTION = 30,

		//more packet types go here

		//not used
		LAST,
	};

	//metadata on the packet itself
	struct Metadata {
		Type type;
		IPaddress srcAddress;
	}meta;

	//info about the server
	struct ServerInformation {
		Metadata meta;
		int networkVersion;
		char name[PACKET_STRING_SIZE];
		int playerCount;
	}serverInfo;

	//info about the client
	struct ClientInformation {
		Metadata meta;
		int clientIndex;
		int accountIndex;
		int characterIndex;
		char username[PACKET_STRING_SIZE];
		//TODO: password
		char handle[PACKET_STRING_SIZE];
		char avatar[PACKET_STRING_SIZE];
	}clientInfo;

	//info about a region
	struct RegionInformation {
		Metadata meta;
		int mapIndex;
		int x, y;
		Region* region;
	}regionInfo;

	//info about a combat scenario
	struct CombatInformation {
		Metadata meta;
		int combatIndex;
		int difficulty;
		//TODO: background image, based on terrain type
		//TODO: array of combatants
		//TODO: rewards
	}combatInfo;

	//info about a character
	struct CharacterInformation {
		Metadata meta;
		int clientIndex;
		int accountIndex;
		int characterIndex;
		char handle[PACKET_STRING_SIZE];
		char avatar[PACKET_STRING_SIZE];
		int mapIndex;
		Vector2 position;
		Vector2 motion;
		Statistics stats;
	}characterInfo;

	//info about an enemy
	struct EnemyInformation {
		Metadata meta;
		char handle[PACKET_STRING_SIZE];
		char avatar[PACKET_STRING_SIZE];
		Statistics stats;
	}enemyInfo;

	//defaults
	SerialPacket() {
		meta.type = Type::NONE;
		meta.srcAddress = {0,0};
	}
};

#endif
