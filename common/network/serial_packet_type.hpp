/* Copyright: (c) Kayne Ruse 2013-2016
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
#pragma once

/* DOCS: The headers indicate what packet type is used for each message
 * different messages under the same header will carry different amounts of
 * valid data, but it will still be carried in that packet's format.
 * FORMAT_* is for internal use, deviding the different format bounds.
*/

enum class SerialPacketType {
	//default: there is something wrong
	NONE = 0,

	//-------------------------
	//ServerPacket
	//  name, player count, version
	//-------------------------

	FORMAT_SERVER = 100,

	//heartbeat
	PING = 101,
	PONG = 102,

	//Used for finding available servers
	BROADCAST_REQUEST = 103,
	BROADCAST_RESPONSE = 104,

	FORMAT_END_SERVER = 199,

	//-------------------------
	//ClientPacket
	//  client index, account index, username
	//-------------------------

	FORMAT_CLIENT = 200,

	//Connecting to a server as a client
	JOIN_REQUEST = 201,
	JOIN_RESPONSE = 202,

	//disconnect from the server
	DISCONNECT_REQUEST = 203,
	DISCONNECT_RESPONSE = 204,
	ADMIN_DISCONNECT_FORCED = 205,

	//load the account
	LOGIN_REQUEST = 206,
	LOGIN_RESPONSE = 207,

	//unload the account
	LOGOUT_REQUEST = 208,
	LOGOUT_RESPONSE = 209,

	//shut down the server
	ADMIN_SHUTDOWN_REQUEST = 210,

	FORMAT_END_CLIENT = 299,

	//-------------------------
	//RegionPacket
	//  room index, x, y, raw data
	//-------------------------

	FORMAT_REGION = 300,

	//map data
	REGION_REQUEST = 301,
	REGION_CONTENT = 302,

	FORMAT_END_REGION = 399,

	//-------------------------
	//CharacterPacket
	//  character index,
	//  handle, avatar,
	//  account index (owner),
	//  room index, origin, motion
	//-------------------------

	FORMAT_CHARACTER = 400,

	//full data update
	CHARACTER_UPDATE = 401,

	//character management
	CHARACTER_CREATE = 402,
	CHARACTER_DELETE = 403,
	CHARACTER_LOAD = 404,
	CHARACTER_UNLOAD = 405,

	//find out info from the server
	QUERY_CHARACTER_EXISTS = 406,

	//actions taken
	CHARACTER_MOVEMENT = 407,

	FORMAT_END_CHARACTER = 499,

	//-------------------------
	//CreaturePacket
	//  creature index,
	//  handle, avatar
	//  bounds
	//  room index, origin, motion
	//-------------------------

	FORMAT_CREATURE = 500,

	//full data update
	CREATURE_UPDATE = 501,

	//character management
	CREATURE_CREATE = 502,
	CREATURE_UNLOAD = 503,

	//find out info from the server
	QUERY_CREATURE_EXISTS = 504,

	//actions taken
	CREATURE_MOVEMENT = 505,

	FORMAT_END_CREATURE = 599,

	//-------------------------
	//TextPacket
	//  name, text
	//-------------------------

	FORMAT_TEXT = 600,

	//general speech
	TEXT_BROADCAST = 601,
	TEXT_SPEECH = 602,
	TEXT_WHISPER = 603,

	//rejection/error messages
	JOIN_REJECTION = 604,
	LOGIN_REJECTION = 605,
	REGION_REJECTION = 606,
	CHARACTER_REJECTION = 607,
	CREATURE_REJECTION = 608,
	SHUTDOWN_REJECTION = 609,
	QUERY_REJECTION = 610,

	FORMAT_END_TEXT = 699,

	//-------------------------
	//not used
	//-------------------------

	LAST = 700
};
