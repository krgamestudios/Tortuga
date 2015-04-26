/* Copyright: (c) Kayne Ruse 2013-2015
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
#include "world.hpp"

#include "channels.hpp"

#include <iostream>

//-------------------------
//static functions
//-------------------------

static int regionChecksum(Region* const region) {
	int sum = 0;
	for(int i = 0; i < REGION_WIDTH; i++) {
		for (int j = 0; j < REGION_HEIGHT; j++) {
			for (int k = 0; k < REGION_DEPTH; k++) {
				sum += region->GetTile(i, j, k);
			}
		}
	}
	return sum;
}

//-------------------------
//map management
//-------------------------

void World::SendRegionRequest(int roomIndex, int x, int y) {
	RegionPacket packet;

	//pack the region's data
	packet.type = SerialPacketType::REGION_REQUEST;
	packet.roomIndex = roomIndex;
	packet.x = x;
	packet.y = y;

	network.SendTo(Channels::SERVER, &packet);
}

void World::hRegionContent(RegionPacket* const argPacket) {
	//debugging
	std::cout << "hRegionContent(" << roomIndex << ", " << argPacket->x << ", " << argPacket->y << ")" << std::endl;

	//checksum
	if (regionChecksum(argPacket->region) == 0) {
		std::cout << "Received checksum failed: " << argPacket->x << ", " << argPacket->y << std::endl;
	}

	//replace existing regions
	regionPager.UnloadIf([&](Region const& region) -> bool {
		return region.GetX() == argPacket->x && region.GetY() == argPacket->y;
	});
	regionPager.PushRegion(argPacket->region);

	//clean up after the serial code
	delete argPacket->region;
	argPacket->region = nullptr;
}

void World::UpdateMap() {
	if (roomIndex == -1) {
		return;
	}

	//these represent the zone of regions that the client needs loaded, including the mandatory buffers (+1/-1)
	int xStart = snapToBase(REGION_WIDTH, camera.x/tileSheet.GetTileW()) - REGION_WIDTH;
	int xEnd = snapToBase(REGION_WIDTH, (camera.x+camera.width)/tileSheet.GetTileW()) + REGION_WIDTH;

	int yStart = snapToBase(REGION_HEIGHT, camera.y/tileSheet.GetTileH()) - REGION_HEIGHT;
	int yEnd = snapToBase(REGION_HEIGHT, (camera.y+camera.height)/tileSheet.GetTileH()) + REGION_HEIGHT;

	//prune distant regions
	regionPager.GetContainer()->remove_if([&](Region const& region) -> bool {
		return region.GetX() < xStart || region.GetX() > xEnd || region.GetY() < yStart || region.GetY() > yEnd;
	});

	//request empty regions within this zone
	for (int i = xStart; i <= xEnd; i += REGION_WIDTH) {
		for (int j = yStart; j <= yEnd; j += REGION_HEIGHT) {
			Region* region = regionPager.FindRegion(i, j);
			if (!region) {
				SendRegionRequest(roomIndex, i, j);
			}
			else if (regionChecksum(region) == 0) {
				std::cout << "Existing checksum failed: " << roomIndex << ", " << i << ", " << j << std::endl;
				SendRegionRequest(roomIndex, i, j);
			}
		}
	}
}