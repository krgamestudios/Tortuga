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
#include "serial_statistics.hpp"

#include "serial_utility.hpp"

void serializeStatistics(void** buffer, Statistics* stats) {
	//integers
	serialCopy(buffer, &stats->level, sizeof(int));
	serialCopy(buffer, &stats->exp, sizeof(int));
	serialCopy(buffer, &stats->maxHP, sizeof(int));
	serialCopy(buffer, &stats->health, sizeof(int));
	serialCopy(buffer, &stats->maxMP, sizeof(int));
	serialCopy(buffer, &stats->mana, sizeof(int));
	serialCopy(buffer, &stats->attack, sizeof(int));
	serialCopy(buffer, &stats->defence, sizeof(int));
	serialCopy(buffer, &stats->intelligence, sizeof(int));
	serialCopy(buffer, &stats->resistance, sizeof(int));
	serialCopy(buffer, &stats->speed, sizeof(int));

	//floats
	serialCopy(buffer, &stats->accuracy, sizeof(float));
	serialCopy(buffer, &stats->evasion, sizeof(float));
	serialCopy(buffer, &stats->luck, sizeof(float));
}

void deserializeStatistics(void** buffer, Statistics* stats) {
	//integers
	deserialCopy(buffer, &stats->level, sizeof(int));
	deserialCopy(buffer, &stats->exp, sizeof(int));
	deserialCopy(buffer, &stats->maxHP, sizeof(int));
	deserialCopy(buffer, &stats->health, sizeof(int));
	deserialCopy(buffer, &stats->maxMP, sizeof(int));
	deserialCopy(buffer, &stats->mana, sizeof(int));
	deserialCopy(buffer, &stats->attack, sizeof(int));
	deserialCopy(buffer, &stats->defence, sizeof(int));
	deserialCopy(buffer, &stats->intelligence, sizeof(int));
	deserialCopy(buffer, &stats->resistance, sizeof(int));
	deserialCopy(buffer, &stats->speed, sizeof(int));

	//floats
	deserialCopy(buffer, &stats->accuracy, sizeof(float));
	deserialCopy(buffer, &stats->evasion, sizeof(float));
	deserialCopy(buffer, &stats->luck, sizeof(float));
}
