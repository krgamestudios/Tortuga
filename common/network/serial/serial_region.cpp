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

#include "map_allocator.hpp"

void serializeRegionFormat(RegionPacket* packet, void* buffer) {
	SERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//format
	SERIALIZE(buffer, &packet->roomIndex, sizeof(int));
	SERIALIZE(buffer, &packet->x, sizeof(int));
	SERIALIZE(buffer, &packet->y, sizeof(int));
}

void serializeRegionContent(RegionPacket* packet, void* buffer) {
	SERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//format
	SERIALIZE(buffer, &packet->roomIndex, sizeof(int));
	SERIALIZE(buffer, &packet->x, sizeof(int));
	SERIALIZE(buffer, &packet->y, sizeof(int));

	//content
	for (register int i = 0; i < REGION_WIDTH; i++) {
		for (register int j = 0; j < REGION_HEIGHT; j++) {
			for (register int k = 0; k < REGION_DEPTH; k++) {
				*reinterpret_cast<Region::type_t*>(buffer) = packet->region->GetTile(i, j, k);
				buffer = reinterpret_cast<char*>(buffer) + sizeof(Region::type_t);
			}
		}
	}
}

void deserializeRegionFormat(RegionPacket* packet, void* buffer) {
	DESERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//format
	DESERIALIZE(buffer, &packet->roomIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->x, sizeof(int));
	DESERIALIZE(buffer, &packet->y, sizeof(int));
}

void deserializeRegionContent(RegionPacket* packet, void* buffer) {
	DESERIALIZE(buffer, &packet->type, sizeof(SerialPacketType));

	//format
	DESERIALIZE(buffer, &packet->roomIndex, sizeof(int));
	DESERIALIZE(buffer, &packet->x, sizeof(int));
	DESERIALIZE(buffer, &packet->y, sizeof(int));

	//an object to work on
	BlankAllocator().Create(
		&packet->region,
		packet->x,
		packet->y
	);

	//content
	for (register int i = 0; i < REGION_WIDTH; i++) {
		for (register int j = 0; j < REGION_HEIGHT; j++) {
			for (register int k = 0; k < REGION_DEPTH; k++) {
				packet->region->SetTile(i, j, k, *reinterpret_cast<Region::type_t*>(buffer));
				buffer = reinterpret_cast<char*>(buffer) + sizeof(Region::type_t);
			}
		}
	}
}