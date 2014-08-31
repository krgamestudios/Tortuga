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

//packet types
#include "character_packet.hpp"
#include "client_packet.hpp"
#include "region_packet.hpp"
#include "server_packet.hpp"

#include <cstring>

//raw memory copy
void serialCopy(void** buffer, void* data, int size) {
	memcpy(*buffer, data, size);
	*buffer = static_cast<char*>(*buffer) + size;
}

void deserialCopy(void** buffer, void* data, int size) {
	memcpy(data, *buffer, size);
	*buffer = static_cast<char*>(*buffer) + size;
}

//simple type functions
void serializeType(SerialPacketBase* packet, void* buffer) {
	serialCopy(&buffer, &packet->type, sizeof(SerialPacketType));
}

void deserializeType(SerialPacketBase* packet, void* buffer) {
	deserialCopy(&buffer, &packet->type, sizeof(SerialPacketType));
}

//main switch functions
void serializePacket(SerialPacketBase* packet, void* buffer) {
	switch(packet->type) {
		//TODO: implement the switch statement
	}
}

void deserializePacket(SerialPacketBase* packet, void* buffer) {
	//find the type, so that you can actually deserialize the packet!
	deserializeType(packet, buffer);
	switch(packet->type) {
		//TODO: implement the switch statement
	}
}