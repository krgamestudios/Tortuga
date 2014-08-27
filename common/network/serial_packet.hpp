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

#include "character_packet.hpp"
#include "client_packet.hpp"
#include "combat_packet.hpp"
#include "enemy_packet.hpp"
#include "region_packet.hpp"
#include "server_packet.hpp"

//NOTE: SerialPacket is defined in serial_packet_base.hpp

union MaxPacket {
	CharacterPacket a;
	ClientPacket b;
	CombatPacket c;
	EnemyPacket d;
	RegionPacket e;
	ServerPacket f;
};
constexpr int MAX_PACKET_SIZE = sizeof(MaxPacket);

/* DOCS: PACKET_BUFFER_SIZE is the memory required to store serialized data
 * DOCS: SerialPacketType::REGION_CONTENT is currently the largest packet type
 * Serialized packet structure:
 *     SerialPacketType
 *     room index
 *     X & Y position
 *     tile data (3 layers)
 *     solid data (bitset)
 * The constants declared here are used for networking ONLY
*/

constexpr int REGION_TILE_FOOTPRINT = sizeof(Region::type_t) * REGION_WIDTH * REGION_HEIGHT * REGION_DEPTH;
constexpr int REGION_SOLID_FOOTPRINT = ceil(REGION_WIDTH * REGION_HEIGHT / 8.0);
constexpr int PACKET_BUFFER_SIZE = sizeof(SerialPacketType) + sizeof(int) * 3 + REGION_TILE_FOOTPRINT + REGION_SOLID_FOOTPRINT;

#endif
