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

void serializeStatistics(Statistics* stats, void* buffer) {
	//integers
	SERIALIZE(buffer, &stats->level, sizeof(int));
	SERIALIZE(buffer, &stats->exp, sizeof(int));
	SERIALIZE(buffer, &stats->maxHP, sizeof(int));
	SERIALIZE(buffer, &stats->health, sizeof(int));
	SERIALIZE(buffer, &stats->maxMP, sizeof(int));
	SERIALIZE(buffer, &stats->mana, sizeof(int));
	SERIALIZE(buffer, &stats->attack, sizeof(int));
	SERIALIZE(buffer, &stats->defence, sizeof(int));
	SERIALIZE(buffer, &stats->intelligence, sizeof(int));
	SERIALIZE(buffer, &stats->resistance, sizeof(int));
	SERIALIZE(buffer, &stats->speed, sizeof(int));

	//floats
	SERIALIZE(buffer, &stats->accuracy, sizeof(float));
	SERIALIZE(buffer, &stats->evasion, sizeof(float));
	SERIALIZE(buffer, &stats->luck, sizeof(float));
}


void deserializeStatistics(Statistics* stats, void* buffer) {
	//integers
	DESERIALIZE(buffer, &stats->level, sizeof(int));
	DESERIALIZE(buffer, &stats->exp, sizeof(int));
	DESERIALIZE(buffer, &stats->maxHP, sizeof(int));
	DESERIALIZE(buffer, &stats->health, sizeof(int));
	DESERIALIZE(buffer, &stats->maxMP, sizeof(int));
	DESERIALIZE(buffer, &stats->mana, sizeof(int));
	DESERIALIZE(buffer, &stats->attack, sizeof(int));
	DESERIALIZE(buffer, &stats->defence, sizeof(int));
	DESERIALIZE(buffer, &stats->intelligence, sizeof(int));
	DESERIALIZE(buffer, &stats->resistance, sizeof(int));
	DESERIALIZE(buffer, &stats->speed, sizeof(int));

	//floats
	DESERIALIZE(buffer, &stats->accuracy, sizeof(float));
	DESERIALIZE(buffer, &stats->evasion, sizeof(float));
	DESERIALIZE(buffer, &stats->luck, sizeof(float));
}
