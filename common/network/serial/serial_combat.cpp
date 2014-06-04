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

void serializeCombat(CombatPacket* packet, void* buffer) {
	SERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//identify the combat instance
	SERIALIZE(buffer, &packet->combatIndex, sizeof(int));
	SERIALIZE(buffer, &packet->difficulty, sizeof(int));
	SERIALIZE(buffer, &packet->terrainType, sizeof(CombatData::Terrain));

	//combatants
	SERIALIZE(buffer, &packet->characterArray, sizeof(int) * COMBAT_MAX_CHARACTERS);
	SERIALIZE(buffer, &packet->enemyArray, sizeof(int) * COMBAT_MAX_ENEMIES);

	//location
	SERIALIZE(buffer, &packet->mapIndex, sizeof(int));
	SERIALIZE(buffer, &packet->origin.x, sizeof(double));
	SERIALIZE(buffer, &packet->origin.y, sizeof(double));

	//TODO: rewards
}

void deserializeCombat(CombatPacket* packet, void* buffer) {
	DESERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//identify the combat instance
	DESERIALIZE(buffer, &packet->combatIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->difficulty, sizeof(int));
	DESERIALIZE(buffer, &packet->terrainType, sizeof(CombatData::Terrain));

	//combatants
	DESERIALIZE(buffer, &packet->characterArray, sizeof(int) * COMBAT_MAX_CHARACTERS);
	DESERIALIZE(buffer, &packet->enemyArray, sizeof(int) * COMBAT_MAX_ENEMIES);

	//location
	DESERIALIZE(buffer, &packet->mapIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->origin.x, sizeof(double));
	DESERIALIZE(buffer, &packet->origin.y, sizeof(double));

	//TODO: rewards
}
