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
#include "server_packet.hpp"

#include "serial_utility.hpp"

void ServerPacket::Serialize(void* buffer) {
	serialize(&buffer, &type, sizeof(SerialPacketType));

	//identify the server
	serialize(&buffer, name, PACKET_STRING_SIZE);
	serialize(&buffer, &playerCount, sizeof(int));
	serialize(&buffer, &version, sizeof(int));
}

void ServerPacket::Deserialize(void* buffer) {
	deserialize(&buffer, &type, sizeof(SerialPacketType));

	//identify the server
	deserialize(&buffer, name, PACKET_STRING_SIZE);
	deserialize(&buffer, &playerCount, sizeof(int));
	deserialize(&buffer, &version, sizeof(int));
}
