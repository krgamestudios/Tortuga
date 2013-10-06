/* Copyright: (c) Kayne Ruse 2013
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
#include "region_pager.hpp"

#include <stdexcept>

RegionPager::RegionPager() {
	//
}

RegionPager::~RegionPager() {
	if (onDelete) {
		for (auto& i : regionList) {
			onDelete(&i);
		}
	}
}

Region* RegionPager::NewRegion(int x, int y) {
	for (auto& i : regionList) {
		if (i.GetX() == x && i.GetY() == y) {
			throw(std::runtime_error("Duplicate Regions detected"));
		}
	}

	regionList.push_front({x, y, regionWidth, regionHeight});
	if (onNew) {
		onNew(&regionList.front());
	}
	return &regionList.front();
}

Region* RegionPager::GetRegion(int x, int y) {
	for (auto& i : regionList) {
		if (i.GetX() == x && i.GetY() == y) {
			return &i;
		}
	}
	//create, insert and return
	regionList.push_front({x, y, regionWidth, regionHeight});
	if (onNew) {
		onNew(&regionList.front());
	}
	return &regionList.front();
}

void RegionPager::DeleteRegion(int x, int y) {
	for (std::list<Region>::iterator i = regionList.begin(); i != regionList.end(); i++) {
		if (i->GetX() == x && i->GetY() == y) {
			if (onDelete) {
				onDelete(&(*i));
			}
			regionList.erase(i);
			break;
		}
	}
}

void RegionPager::DrawTo(SDL_Surface* const dest, std::list<TileSheet>* const sheetList, int camX, int camY) {
	/* for each region:
	 *     for each tile within that reagon
	 *         if you have the correct tile sheet
	 *             draw to the screen
	 *         else
	 *             error
	*/

	for (auto& regionIter : regionList) {
		//draw the region's location
		SDL_Rect box = {
			Sint16(regionIter.GetX() + camX),
			Sint16(regionIter.GetY() + camY),
			Uint16(regionIter.GetWidth()),
			Uint16(regionIter.GetHeight())
		};
		SDL_FillRect(dest, &box, SDL_MapRGB(dest->format, 10, 10, 20));

		//draw each tile
		for (auto& tileIter : *regionIter.GetTiles()) {
			for (auto& sheetIter : *sheetList) {
				if (sheetIter.InRange(tileIter.tileIndex)) {
					sheetIter.DrawTo(
						dest,
						tileIter.x + regionIter.GetX() + camX,
						tileIter.y + regionIter.GetY() + camY,
						tileIter.tileIndex
					);
					//skip to the next tile
					goto continueTile;
				}
			}

			//reaching this point without rendering means that the tile is invalid
			throw(std::runtime_error("Undrawable tile encountered"));

		continueTile: ;
			//continue with the next tile
		}
	}
}