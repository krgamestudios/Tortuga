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
#include "region_packet.hpp"

#include "serial_utility.hpp"

void RegionPacket::Serialize(void* buffer) {
	serializeCopy(&buffer, &type, sizeof(SerialPacketType));

	//format
	serializeCopy(&buffer, &roomIndex, sizeof(int));
	serializeCopy(&buffer, &x, sizeof(int));
	serializeCopy(&buffer, &y, sizeof(int));

	if (type != SerialPacketType::REGION_CONTENT) {
		return;
	}

	//tiles
	for (int i = 0; i < REGION_WIDTH; i++) {
		for (int j = 0; j < REGION_HEIGHT; j++) {
			for (int k = 0; k < REGION_DEPTH; k++) {
				*reinterpret_cast<Region::type_t*>(buffer) = region->GetTile(i, j, k);
				buffer = reinterpret_cast<char*>(buffer) + sizeof(Region::type_t);
			}
		}
	}

	//solids
	serializeCopy(&buffer, region->GetSolidBitset(), REGION_SOLID_FOOTPRINT);
}

void RegionPacket::Deserialize(void* buffer) {
	deserializeCopy(&buffer, &type, sizeof(SerialPacketType));

	//format
	deserializeCopy(&buffer, &roomIndex, sizeof(int));
	deserializeCopy(&buffer, &x, sizeof(int));
	deserializeCopy(&buffer, &y, sizeof(int));

	if (type != SerialPacketType::REGION_CONTENT) {
		return;
	}

	//an object to work on
	region = new Region(x, y);

	//tiles
	for (int i = 0; i < REGION_WIDTH; i++) {
		for (int j = 0; j < REGION_HEIGHT; j++) {
			for (int k = 0; k < REGION_DEPTH; k++) {
				region->SetTile(i, j, k, *reinterpret_cast<Region::type_t*>(buffer));
				buffer = reinterpret_cast<char*>(buffer) + sizeof(Region::type_t);
			}
		}
	}

	//solids
	deserializeCopy(&buffer, region->GetSolidBitset(), REGION_SOLID_FOOTPRINT);
}