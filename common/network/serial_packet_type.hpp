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

/* DOCS: The headers indicate what packet type is used for each message
 * different messages under the same header will carry different amounts of
 * valid data, but it will still be carried in that packet's format.
*/

//TODO: This needs to be smoothed out
enum class SerialPacketType {
	//default: there is something wrong
	NONE = 0,

	//-------------------------
	//ServerPacket
	//  name, player count, version
	//-------------------------

	//heartbeat
	PING = 1,
	PONG = 2,

	//Used for finding available servers
	BROADCAST_REQUEST = 3,
	BROADCAST_RESPONSE = 4,

	//-------------------------
	//ClientPacket
	//  client index, account index, username
	//-------------------------

	//Connecting to a server as a client
	JOIN_REQUEST = 5,
	JOIN_RESPONSE = 6,

	//disconnect from the server
	DISCONNECT_REQUEST = 7,
	DISCONNECT_RESPONSE = 8,
	DISCONNECT_FORCED = 9,

	//load the account
	LOGIN_REQUEST = 10,
	LOGIN_RESPONSE = 11,

	//unload the account
	LOGOUT_REQUEST = 12,
	LOGOUT_RESPONSE = 13,

	//shut down the server
	SHUTDOWN_REQUEST = 14,

	//-------------------------
	//RegionPacket
	//  room index, x, y, raw data
	//-------------------------

	//map data
	REGION_REQUEST = 15, //NOTE: technically a query
	REGION_CONTENT = 16,

	//-------------------------
	//CharacterPacket
	//  character index,
	//  handle, avatar,
	//  account index (owner),
	//  room index, origin, motion,
	//  statistics
	//-------------------------

	//character management
	CHARACTER_CREATE = 17,
	CHARACTER_DELETE = 18,
	CHARACTER_LOAD = 19,
	CHARACTER_UNLOAD = 20,

	//find out info from the server
	QUERY_CHARACTER_EXISTS = 21,
	QUERY_CHARACTER_STATS = 22,
	QUERY_CHARACTER_LOCATION = 23,

	//set the info in the server
	CHARACTER_SET_ROOM = 24,
	CHARACTER_SET_ORIGIN = 25,
	CHARACTER_SET_MOTION = 26,

	//TODO: enemy management

	//-------------------------
	//TextPacket
	//  name, text
	//-------------------------

	//general speech
	TEXT_BROADCAST = 27,

	//rejection/error messages
	JOIN_REJECTION = 28,
	LOGIN_REJECTION = 29,
	REGION_REJECTION = 30,
	CHARACTER_REJECTION = 31,
	SHUTDOWN_REJECTION = 32,

	//-------------------------
	//not used
	//-------------------------

	LAST = 33
};

#endif