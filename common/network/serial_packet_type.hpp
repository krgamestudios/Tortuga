/* Copyright: (c) Kayne Ruse 2013-2015
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

/* DOCS: The headers indicate what packet type is used for each message
 * different messages under the same header will carry different amounts of
 * valid data, but it will still be carried in that packet's format.
*/

//TODO: This needs to be smoothed out
enum class SerialPacketType {
	//default: there is something wrong
	NONE,

	//-------------------------
	//ServerPacket
	//  name, player count, version
	//-------------------------

	//heartbeat
	PING,
	PONG,

	//Used for finding available servers
	BROADCAST_REQUEST,
	BROADCAST_RESPONSE,

	//-------------------------
	//ClientPacket
	//  client index, account index, username
	//-------------------------

	//Connecting to a server as a client
	JOIN_REQUEST,
	JOIN_RESPONSE,

	//disconnect from the server
	DISCONNECT_REQUEST,
	DISCONNECT_RESPONSE,
	DISCONNECT_FORCED,

	//load the account
	LOGIN_REQUEST,
	LOGIN_RESPONSE,

	//unload the account
	LOGOUT_REQUEST,
	LOGOUT_RESPONSE,

	//shut down the server
	SHUTDOWN_REQUEST,

	//-------------------------
	//RegionPacket
	//  room index, x, y, raw data
	//-------------------------

	//map data
	REGION_REQUEST, //NOTE: technically a query
	REGION_CONTENT,

	//-------------------------
	//CharacterPacket
	//  character index,
	//  handle, avatar,
	//  account index (owner),
	//  room index, origin, motion,
	//  statistics
	//-------------------------

	//character management
	CHARACTER_CREATE,
	CHARACTER_DELETE,
	CHARACTER_LOAD,
	CHARACTER_UNLOAD,

	//find out info from the server
	QUERY_CHARACTER_EXISTS,
	QUERY_CHARACTER_STATS,
	QUERY_CHARACTER_LOCATION,

	//set the info in the server
	CHARACTER_MOVEMENT,
	CHARACTER_ATTACK,

	//admin control
//	ADMIN_SET_CHARACTER_ORIGIN,

	//-------------------------
	//MonsterPacket
	//  monster index,
	//  handle, avatar, hitbox
	//  room index, origin, motion
	//  TODO: attack data
	//-------------------------

	MONSTER_CREATE,
	MONSTER_DELETE,

	QUERY_MONSTER_EXISTS, //a list of monsters in a room
	QUERY_MONSTER_STATS, //statistics of a specific monster type or instance
	QUERY_MONSTER_LOCATION, //umm...

	MONSTER_MOVEMENT, //monster movement
	MONSTER_ATTACK,

	//-------------------------
	//TextPacket
	//  name, text
	//-------------------------

	//general speech
	TEXT_BROADCAST,

	//rejection/error messages
	JOIN_REJECTION,
	LOGIN_REJECTION,
	REGION_REJECTION,
	CHARACTER_REJECTION,
	MONSTER_REJECTION,
	SHUTDOWN_REJECTION,

	//-------------------------
	//not used
	//-------------------------

	LAST
};

#endif