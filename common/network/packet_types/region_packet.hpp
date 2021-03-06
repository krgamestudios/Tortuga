/* Copyright: (c) Kayne Ruse 2013-2016
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
#pragma once

#include "serial_packet_base.hpp"

#include "region.hpp"

#include <cmath>

//define the memory footprint for the region's members
constexpr int REGION_TILE_FOOTPRINT = sizeof(Region::type_t) * REGION_WIDTH * REGION_HEIGHT * REGION_DEPTH;
constexpr int REGION_SOLID_FOOTPRINT = ceil(REGION_WIDTH * REGION_HEIGHT / 8.0);
constexpr int REGION_METADATA_FOOTPRINT = sizeof(int) * 3;

struct RegionPacket : SerialPacketBase {
	//location/identify the region
	int roomIndex;
	int x, y;

	//the data
	Region* region;
};

void serializeRegion(void* buffer, RegionPacket* packet);
void deserializeRegion(void* buffer, RegionPacket* packet);
