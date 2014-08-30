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

#include "serial_packet_type.hpp"

#include <cstring>

void serializePacket(SerialPacketBase* packet, void* data) {
	switch(packet->GetType()) {
		//define all packet types
	}
}

void deserializePacket(SerialPacketBase* packet, void* data) {
	//get the type
	SerialPacketType type;
	memcpy(&type, data, sizeof(SerialPacketType));

	switch(type) {
		//define all extra types
	}
}

void serializeCopy(void** bufferHead, void* data, int size) {
	memcpy(*bufferHead, data, size);
	(*bufferHead) = static_cast<void*>(static_cast<char*>(*bufferHead) + size);
}

void deserializeCopy(void** bufferHead, void* data, int size) {
	memcpy(data, *bufferHead, size);
	(*bufferHead) = static_cast<void*>(static_cast<char*>(*bufferHead) + size);
}

void serializeCopyStatistics(void** bufferHead, Statistics* stats) {
	//integers
	serializeCopy(bufferHead, &stats->level, sizeof(int));
	serializeCopy(bufferHead, &stats->exp, sizeof(int));
	serializeCopy(bufferHead, &stats->maxHP, sizeof(int));
	serializeCopy(bufferHead, &stats->health, sizeof(int));
	serializeCopy(bufferHead, &stats->maxMP, sizeof(int));
	serializeCopy(bufferHead, &stats->mana, sizeof(int));
	serializeCopy(bufferHead, &stats->attack, sizeof(int));
	serializeCopy(bufferHead, &stats->defence, sizeof(int));
	serializeCopy(bufferHead, &stats->intelligence, sizeof(int));
	serializeCopy(bufferHead, &stats->resistance, sizeof(int));
	serializeCopy(bufferHead, &stats->speed, sizeof(int));

	//floats
	serializeCopy(bufferHead, &stats->accuracy, sizeof(float));
	serializeCopy(bufferHead, &stats->evasion, sizeof(float));
	serializeCopy(bufferHead, &stats->luck, sizeof(float));
}

void deserializeCopyStatistics(void** bufferHead, Statistics* stats) {
	//integers
	deserializeCopy(bufferHead, &stats->level, sizeof(int));
	deserializeCopy(bufferHead, &stats->exp, sizeof(int));
	deserializeCopy(bufferHead, &stats->maxHP, sizeof(int));
	deserializeCopy(bufferHead, &stats->health, sizeof(int));
	deserializeCopy(bufferHead, &stats->maxMP, sizeof(int));
	deserializeCopy(bufferHead, &stats->mana, sizeof(int));
	deserializeCopy(bufferHead, &stats->attack, sizeof(int));
	deserializeCopy(bufferHead, &stats->defence, sizeof(int));
	deserializeCopy(bufferHead, &stats->intelligence, sizeof(int));
	deserializeCopy(bufferHead, &stats->resistance, sizeof(int));
	deserializeCopy(bufferHead, &stats->speed, sizeof(int));

	//floats
	deserializeCopy(bufferHead, &stats->accuracy, sizeof(float));
	deserializeCopy(bufferHead, &stats->evasion, sizeof(float));
	deserializeCopy(bufferHead, &stats->luck, sizeof(float));
}
