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
#include "serial.hpp"

#include "serial_util.hpp"

void serializeCharacter(CharacterPacket* packet, void* buffer) {
	SERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//identify the character
	SERIALIZE(buffer, &packet->characterIndex, sizeof(int));
	SERIALIZE(buffer, &packet->handle, PACKET_STRING_SIZE);
	SERIALIZE(buffer, &packet->avatar, PACKET_STRING_SIZE);

	//the owner
	SERIALIZE(buffer, &packet->accountIndex, sizeof(int));

	//location
	SERIALIZE(buffer, &packet->roomIndex, sizeof(int));
	SERIALIZE(buffer, &packet->origin.x, sizeof(double));
	SERIALIZE(buffer, &packet->origin.y, sizeof(double));
	SERIALIZE(buffer, &packet->motion.x, sizeof(double));
	SERIALIZE(buffer, &packet->motion.y, sizeof(double));

	//stats structure
	serializeStatistics(&packet->stats, buffer);
	buffer = reinterpret_cast<char*>(buffer) + sizeof(Statistics);

	//TODO: equipment
	//TODO: items
	//TODO: buffs
	//TODO: debuffs
}

void deserializeCharacter(CharacterPacket* packet, void* buffer) {
	DESERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//identify the character
	DESERIALIZE(buffer, &packet->characterIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->handle, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, &packet->avatar, PACKET_STRING_SIZE);

	//the owner
	DESERIALIZE(buffer, &packet->accountIndex, sizeof(int));

	//location
	DESERIALIZE(buffer, &packet->roomIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->origin.x, sizeof(double));
	DESERIALIZE(buffer, &packet->origin.y, sizeof(double));
	DESERIALIZE(buffer, &packet->motion.x, sizeof(double));
	DESERIALIZE(buffer, &packet->motion.y, sizeof(double));

	//stats structure
	deserializeStatistics(&packet->stats, buffer);
	buffer = reinterpret_cast<char*>(buffer) + sizeof(Statistics);

	//TODO: equipment
	//TODO: items
	//TODO: buffs
	//TODO: debuffs
}
