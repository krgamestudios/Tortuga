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
#include "serial_utility.hpp"

#include <cstring>

void serialize(void** bufferHead, void* data, int size) {
	memcpy(*bufferHead, data, size);
	(*bufferHead) = static_cast<void*>(static_cast<char*>(*bufferHead) + size);
}

void deserialize(void** bufferHead, void* data, int size) {
	memcpy(data, *bufferHead, size);
	(*bufferHead) = static_cast<void*>(static_cast<char*>(*bufferHead) + size);
}

void serializeStatistics(void** bufferHead, Statistics* stats) {
	//integers
	serialize(bufferHead, &stats->level, sizeof(int));
	serialize(bufferHead, &stats->exp, sizeof(int));
	serialize(bufferHead, &stats->maxHP, sizeof(int));
	serialize(bufferHead, &stats->health, sizeof(int));
	serialize(bufferHead, &stats->maxMP, sizeof(int));
	serialize(bufferHead, &stats->mana, sizeof(int));
	serialize(bufferHead, &stats->attack, sizeof(int));
	serialize(bufferHead, &stats->defence, sizeof(int));
	serialize(bufferHead, &stats->intelligence, sizeof(int));
	serialize(bufferHead, &stats->resistance, sizeof(int));
	serialize(bufferHead, &stats->speed, sizeof(int));

	//floats
	serialize(bufferHead, &stats->accuracy, sizeof(float));
	serialize(bufferHead, &stats->evasion, sizeof(float));
	serialize(bufferHead, &stats->luck, sizeof(float));
}

void deserializeStatistics(void** bufferHead, Statistics* stats) {
	//integers
	deserialize(bufferHead, &stats->level, sizeof(int));
	deserialize(bufferHead, &stats->exp, sizeof(int));
	deserialize(bufferHead, &stats->maxHP, sizeof(int));
	deserialize(bufferHead, &stats->health, sizeof(int));
	deserialize(bufferHead, &stats->maxMP, sizeof(int));
	deserialize(bufferHead, &stats->mana, sizeof(int));
	deserialize(bufferHead, &stats->attack, sizeof(int));
	deserialize(bufferHead, &stats->defence, sizeof(int));
	deserialize(bufferHead, &stats->intelligence, sizeof(int));
	deserialize(bufferHead, &stats->resistance, sizeof(int));
	deserialize(bufferHead, &stats->speed, sizeof(int));

	//floats
	deserialize(bufferHead, &stats->accuracy, sizeof(float));
	deserialize(bufferHead, &stats->evasion, sizeof(float));
	deserialize(bufferHead, &stats->luck, sizeof(float));
}
