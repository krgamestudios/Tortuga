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

#include "character_data.hpp"
#include "creature_data.hpp"
#include "serial_packet.hpp"
#include "vector2.hpp"

void fullClientUnload(int index);
void fullAccountUnload(int index);
void fullCharacterUnload(int index);

void pumpPacket(SerialPacket* const argPacket);
void pumpPacketProximity(SerialPacket* const argPacket, int roomIndex, Vector2 position = {0, 0}, int distance = -1);

void copyCharacterToPacket(CharacterPacket* const packet, int characterIndex);
void copyCharacterToPacket(CharacterPacket* const packet, CharacterData* const characterData, int characterIndex);

void copyCreatureToPacket(CreaturePacket* const packet, CreatureData* const creatureData, int creatureIndex);

void pumpAndChangeRooms(int characterIndex, int newRoomIndex);
void pumpAndChangeRooms(CharacterData* const characterData, int newRoomIndex, int characterIndex);

double distance(Vector2 lhs, Vector2 rhs);
