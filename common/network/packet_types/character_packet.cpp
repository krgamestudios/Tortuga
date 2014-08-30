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
	serializeCopy(&buffer, &type, sizeof(SerialPacketType));

	//identify the character
	serializeCopy(&buffer, &characterIndex, sizeof(int));
	serializeCopy(&buffer, handle, PACKET_STRING_SIZE);
	serializeCopy(&buffer, avatar, PACKET_STRING_SIZE);

	//the owner
	serializeCopy(&buffer, &accountIndex, sizeof(int));

	//location
	serializeCopy(&buffer, &roomIndex, sizeof(int));
	serializeCopy(&buffer, &origin.x, sizeof(double));
	serializeCopy(&buffer, &origin.y, sizeof(double));
	serializeCopy(&buffer, &motion.x, sizeof(double));
	serializeCopy(&buffer, &motion.y, sizeof(double));

	//stats structure
	serializeCopyStatistics(&buffer, &stats);

	//TODO: gameplay components: equipment, items, buffs, debuffs
}

void CharacterPacket::Deserialize(void* buffer) {
	deserializeCopy(&buffer, &type, sizeof(SerialPacketType));

	//identify the character
	deserializeCopy(&buffer, &characterIndex, sizeof(int));
	deserializeCopy(&buffer, handle, PACKET_STRING_SIZE);
	deserializeCopy(&buffer, avatar, PACKET_STRING_SIZE);

	//the owner
	deserializeCopy(&buffer, &accountIndex, sizeof(int));

	//location
	deserializeCopy(&buffer, &roomIndex, sizeof(int));
	deserializeCopy(&buffer, &origin.x, sizeof(double));
	deserializeCopy(&buffer, &origin.y, sizeof(double));
	deserializeCopy(&buffer, &motion.x, sizeof(double));
	deserializeCopy(&buffer, &motion.y, sizeof(double));

	//stats structure
	deserializeCopyStatistics(&buffer, &stats);

	//TODO: gameplay components: equipment, items, buffs, debuffs
}
