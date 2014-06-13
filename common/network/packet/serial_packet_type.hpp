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
#ifndef SERIALPACKETTYPE_HPP_
#define SERIALPACKETTYPE_HPP_

/* Key for the comments:
 * request => response
*/

enum class SerialPacketType {
	//default: there is something wrong
	NONE = 0,

	//keep alive
	//ping => pong
	PING = 1,
	PONG = 2,

	//search for the server list
	//none => server name, player count, version info (and source address)
	BROADCAST_REQUEST = 3,
	BROADCAST_RESPONSE = 4,

	//try to join the server
	//username, and password => client index, account index
	JOIN_REQUEST = 5,
	JOIN_RESPONSE = 6,
	JOIN_REJECTION = 7,

	//mass update of all surrounding content
	//character.x, character.y => packet barrage
	SYNCHRONIZE = 8,

	//disconnect from the server
	//autentication, account index => disconnect that account
	DISCONNECT = 9,

	//shut down the server
	//autentication => disconnect, shutdown
	SHUTDOWN = 10,

	//map data
	//room index, region.x, region.y => room index, region.x, region.y, region content
	REGION_REQUEST = 11,
	REGION_CONTENT = 12,

	//combat data
	//TODO: system incomplete
	COMBAT_NEW = 13,
	COMBAT_DELETE = 14,
	COMBAT_UPDATE = 15,

	COMBAT_ENTER_REQUEST = 16,
	COMBAT_ENTER_RESPONSE = 17,

	COMBAT_EXIT_REQUEST = 18,
	COMBAT_EXIT_RESPONSE = 19,

	//TODO: COMBAT info

	COMBAT_REJECTION = 20,

	//character data
	//character data => etc.
	CHARACTER_NEW = 21,
	CHARACTER_DELETE = 22,
	CHARACTER_UPDATE = 23,

	//authentication, character index => character stats
	CHARACTER_STATS_REQUEST= 24,
	CHARACTER_STATS_RESPONSE = 25,

	//character new => character rejection, disconnect?
	CHARACTER_REJECTION = 26,

	//enemy data
	//enemy data => etc.
	ENEMY_NEW = 27,
	ENEMY_DELETE = 28,
	ENEMY_UPDATE = 29,

	ENEMY_STATS_REQUEST = 30,
	ENEMY_STATS_RESPONSE = 31,

	//enemy index => enemy doens't exist
	ENEMY_REJECTION= 32,

	//NOTE: more packet types go here

	//not used
	LAST
};

#endif