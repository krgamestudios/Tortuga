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
#include "character_packet.hpp"

#include "serial_utility.hpp"

#include "serial_statistics.hpp"

void serializeCharacter(void* buffer, CharacterPacket* packet) {
	serialCopy(&buffer, &packet->type, sizeof(SerialPacketType));

	//identify the character
	serialCopy(&buffer, &packet->characterIndex, sizeof(int));
	serialCopy(&buffer, packet->handle, PACKET_STRING_SIZE);
	serialCopy(&buffer, packet->avatar, PACKET_STRING_SIZE);

	//the owner
	serialCopy(&buffer, &packet->accountIndex, sizeof(int));

	//location
	serialCopy(&buffer, &packet->roomIndex, sizeof(int));
	serialCopy(&buffer, &packet->origin.x, sizeof(double));
	serialCopy(&buffer, &packet->origin.y, sizeof(double));
	serialCopy(&buffer, &packet->motion.x, sizeof(double));
	serialCopy(&buffer, &packet->motion.y, sizeof(double));

	//stats structure
	serializeStatistics(&buffer, &packet->stats);

	//gameplay components: equipment, items, buffs, debuffs...
}

void deserializeCharacter(void* buffer, CharacterPacket* packet) {
	deserialCopy(&buffer, &packet->type, sizeof(SerialPacketType));

	//identify the character
	deserialCopy(&buffer, &packet->characterIndex, sizeof(int));
	deserialCopy(&buffer, packet->handle, PACKET_STRING_SIZE);
	deserialCopy(&buffer, packet->avatar, PACKET_STRING_SIZE);

	//the owner
	deserialCopy(&buffer, &packet->accountIndex, sizeof(int));

	//location
	deserialCopy(&buffer, &packet->roomIndex, sizeof(int));
	deserialCopy(&buffer, &packet->origin.x, sizeof(double));
	deserialCopy(&buffer, &packet->origin.y, sizeof(double));
	deserialCopy(&buffer, &packet->motion.x, sizeof(double));
	deserialCopy(&buffer, &packet->motion.y, sizeof(double));

	//stats structure
	deserializeStatistics(&buffer, &packet->stats);

	//gameplay components: equipment, items, buffs, debuffs...
}
