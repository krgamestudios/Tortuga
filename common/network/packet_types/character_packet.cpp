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

void CharacterPacket::Serialize(void* buffer) {
	serialize(&buffer, &type, sizeof(SerialPacketType));

	//identify the character
	serialize(&buffer, &characterIndex, sizeof(int));
	serialize(&buffer, handle, PACKET_STRING_SIZE);
	serialize(&buffer, avatar, PACKET_STRING_SIZE);

	//the owner
	serialize(&buffer, &accountIndex, sizeof(int));

	//location
	serialize(&buffer, &roomIndex, sizeof(int));
	serialize(&buffer, &origin.x, sizeof(double));
	serialize(&buffer, &origin.y, sizeof(double));
	serialize(&buffer, &motion.x, sizeof(double));
	serialize(&buffer, &motion.y, sizeof(double));

	//stats structure
	serializeStatistics(&buffer, &stats);

	//TODO: gameplay components: equipment, items, buffs, debuffs
}

void CharacterPacket::Deserialize(void* buffer) {
	deserialize(&buffer, &type, sizeof(SerialPacketType));

	//identify the character
	deserialize(&buffer, &characterIndex, sizeof(int));
	deserialize(&buffer, handle, PACKET_STRING_SIZE);
	deserialize(&buffer, avatar, PACKET_STRING_SIZE);

	//the owner
	deserialize(&buffer, &accountIndex, sizeof(int));

	//location
	deserialize(&buffer, &roomIndex, sizeof(int));
	deserialize(&buffer, &origin.x, sizeof(double));
	deserialize(&buffer, &origin.y, sizeof(double));
	deserialize(&buffer, &motion.x, sizeof(double));
	deserialize(&buffer, &motion.y, sizeof(double));

	//stats structure
	deserializeStatistics(&buffer, &stats);

	//TODO: gameplay components: equipment, items, buffs, debuffs
}
