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
#ifndef SERIALPACKET_HPP_
#define SERIALPACKET_HPP_

#include "serial_packet_base.hpp"
#include "character_packet.hpp"
#include "client_packet.hpp"
#include "region_packet.hpp"
#include "server_packet.hpp"
#include "text_packet.hpp"

//SerialPacketBase is defined in serial_packet_base.hpp
typedef SerialPacketBase SerialPacket;

//DOCS: NETWORK_VERSION is used to discern compatible servers and clients
constexpr int NETWORK_VERSION = -1;

union MaxPacket {
	CharacterPacket a;
	ClientPacket b;
	RegionPacket c;
	ServerPacket d;
	TextPacket e;
};

constexpr int MAX_PACKET_SIZE = sizeof(MaxPacket);

/* DOCS: PACKET_BUFFER_SIZE is the memory required to store serialized data
 * DOCS: SerialPacketType::REGION_CONTENT is currently the largest packet type
 * Serialized RegionPacket structure:
 *     SerialPacketType
 *     room index (int)
 *     X & Y position (int)
 *     tile data (3 layers)
 *     solid data (bitset)
*/

constexpr int PACKET_BUFFER_SIZE =
	sizeof(SerialPacketType) +
	REGION_METADATA_FOOTPRINT +
	REGION_TILE_FOOTPRINT +
	REGION_SOLID_FOOTPRINT;

#endif
