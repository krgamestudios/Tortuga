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
	PING,
	PONG,

	//search for the server list
	//none => server name, player count, version info (and source address)
	BROADCAST_REQUEST,
	BROADCAST_RESPONSE,

	//try to join the server
	//username, and password => client index, account index
	JOIN_REQUEST,
	JOIN_RESPONSE,
	JOIN_REJECTION,

	//mass update of all surrounding content
	//character.x, character.y => packet barrage
	SYNCHRONIZE,

	//disconnect from the server
	//autentication, account index => disconnect that account
	DISCONNECT,

	//shut down the server
	//autentication => disconnect, shutdown
	SHUTDOWN,

	//map data
	//room index, region.x, region.y => room index, region.x, region.y, region content
	REGION_REQUEST,
	REGION_CONTENT,

	//combat data
	//TODO: system incomplete
	COMBAT_NEW,
	COMBAT_DELETE,
	COMBAT_UPDATE,

	COMBAT_ENTER_REQUEST,
	COMBAT_ENTER_RESPONSE,

	COMBAT_EXIT_REQUEST,
	COMBAT_EXIT_RESPONSE,

	//TODO: COMBAT info

	COMBAT_REJECTION,

	//character data
	//character data => etc.
	CHARACTER_NEW,
	CHARACTER_DELETE,
	CHARACTER_UPDATE,

	//authentication, character index => character stats
	CHARACTER_STATS_REQUEST,
	CHARACTER_STATS_RESPONSE,

	//character new => character rejection, disconnect?
	CHARACTER_REJECTION,

	//enemy data
	//enemy data => etc.
	ENEMY_NEW,
	ENEMY_DELETE,
	ENEMY_UPDATE,

	ENEMY_STATS_REQUEST,
	ENEMY_STATS_RESPONSE,

	//enemy index => enemy doens't exist
	ENEMY_REJECTION,

	//NOTE: more packet types go here

	//not used
	LAST
};

#endif