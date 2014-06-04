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

void serializeEnemy(EnemyPacket* packet, void* buffer) {
	SERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//identify the enemy
	SERIALIZE(buffer, &packet->enemyIndex, sizeof(int));
	SERIALIZE(buffer, &packet->handle, PACKET_STRING_SIZE);
	SERIALIZE(buffer, &packet->avatar, PACKET_STRING_SIZE);

	//gameplay

	//stats structure
	serializeStatistics(&packet->stats, buffer);
	buffer = reinterpret_cast<char*>(buffer) + sizeof(Statistics);

	//TODO: equipment
	//TODO: items
	//TODO: buffs
	//TODO: debuffs

	//TODO: rewards
}

void deserializeEnemy(EnemyPacket* packet, void* buffer) {
	DESERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//identify the enemy
	DESERIALIZE(buffer, &packet->enemyIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->handle, PACKET_STRING_SIZE);
	DESERIALIZE(buffer, &packet->avatar, PACKET_STRING_SIZE);

	//stats structure
	deserializeStatistics(&packet->stats, buffer);
	buffer = reinterpret_cast<char*>(buffer) + sizeof(Statistics);

	//TODO: equipment
	//TODO: items
	//TODO: buffs
	//TODO: debuffs

	//TODO: rewards
}
