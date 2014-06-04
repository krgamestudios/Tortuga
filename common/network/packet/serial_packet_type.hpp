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

enum class SerialPacketType {
	//default: there is something wrong
	NONE = 0,

	//keep alive
	PING,
	PONG,

	//searching for a server to join
	BROADCAST_REQUEST,
	BROADCAST_RESPONSE,
	BROADCAST_REJECTION,

	//try to join the server
	JOIN_REQUEST,
	JOIN_RESPONSE,
	JOIN_REJECTION,

	//mass update
	SYNCHRONIZE,

	//disconnect from the server
	DISCONNECT,

	//shut down the server
	SHUTDOWN,

	//map data
	REGION_REQUEST,
	REGION_CONTENT,
	REGION_REJECTION,

	//combat data
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
	CHARACTER_NEW,
	CHARACTER_DELETE,
	CHARACTER_UPDATE,

	CHARACTER_STATS_REQUEST,
	CHARACTER_STATS_RESPONSE,

	CHARACTER_REJECTION,

	//enemy data
	ENEMY_NEW,
	ENEMY_DELETE,
	ENEMY_UPDATE,

	ENEMY_STATS_REQUEST,
	ENEMY_STATS_RESPONSE,

	ENEMY_REJECTION,

	//more packet types go here

	//not used
	LAST,
};

#endif