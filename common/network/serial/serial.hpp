/* Copyright: (c) Kayne Ruse 2014
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
#ifndef SERIALIZE_HPP_
#define SERIALIZE_HPP_

#include "serial_packet.hpp"

#include "region.hpp"
#include "statistics.hpp"

//Primary interface functions
void serializePacket(SerialPacketBase*, void* dest);
void deserializePacket(SerialPacketBase*, void* src);

void serializeType(SerialPacketBase*, void*);
void deserializeType(SerialPacketBase*, void*);

//utility functions, exposed
void serializeCharacter(CharacterPacket*, void*);
void serializeClient(ClientPacket*, void*);
void serializeCombat(CombatPacket*, void*);
void serializeEnemy(EnemyPacket*, void*);
void serializeRegionFormat(RegionPacket*, void*);
void serializeRegionContent(RegionPacket*, void*);
void serializeServer(ServerPacket*, void*);
void serializeStatistics(Statistics*, void*);

void deserializeCharacter(CharacterPacket*, void*);
void deserializeClient(ClientPacket*, void*);
void deserializeCombat(CombatPacket*, void*);
void deserializeEnemy(EnemyPacket*, void*);
void deserializeRegionFormat(RegionPacket*, void*);
void deserializeRegionContent(RegionPacket*, void*);
void deserializeServer(ServerPacket*, void*);
void deserializeStatistics(Statistics*, void*);

/* DOCS: Keep PACKET_BUFFER_SIZE up to date
 * DOCS: SerialPacketType::REGION_CONTENT is currently the largest type of packet, read more
 * The metadata used are:
 *     SerialPacketType
 *     room index
 *     X & Y positon
 * The rest is taken up by the Regions's content.
*/

constexpr int PACKET_BUFFER_SIZE = sizeof(SerialPacketType) + sizeof(int) * 3 + REGION_FOOTPRINT;

#endif