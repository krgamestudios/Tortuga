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
#include "tile_sheet.hpp"

TileSheet& TileSheet::operator=(TileSheet const& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	Free();

	//Copy the other TileSheet's stuff
	texture = rhs.texture;
	clip = rhs.clip;
	local = false;
	countX = rhs.countX;
	countY = rhs.countY;
}

TileSheet& TileSheet::operator=(TileSheet&& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	Free();

	//Copy the other TileSheet's stuff
	texture = rhs.texture;
	clip = rhs.clip;
	local = false;
	countX = rhs.countX;
	countY = rhs.countY;

	rhs.texture = nullptr;
	rhs.clip = {0, 0, 0, 0};
	rhs.local = false;
	rhs.countX = 0;
	rhs.countY = 0;
}

void TileSheet::Load(SDL_Renderer* renderer, std::string fname, int tileWidth, int tileHeight) {
	Image::Load(renderer, fname);
	countX = clip.w / tileWidth;
	countY = clip.h / tileHeight;
	clip.w = tileWidth;
	clip.h = tileHeight;
}

SDL_Texture* TileSheet::SetTexture(SDL_Texture* ptr, int tileWidth, int tileHeight) {
	Image::SetTexture(ptr);
	countX = clip.w / tileWidth;
	countY = clip.h / tileHeight;
	clip.w = tileWidth;
	clip.h = tileHeight;
}

void TileSheet::Free() {
	Image::Free();
	countX = countY = 0;
}

void TileSheet::DrawLayerTo(SDL_Renderer* const renderer, Region* const region, int layer, int camX, int camY, double scaleX, double scaleY) {
	//TODO: (2) empty
}

void TileSheet::DrawRegionTo(SDL_Renderer* const renderer, Region* const region, int camX, int camY, double scaleX, double scaleY) {
	//TODO: (2) make TileSheet a friend class of Region
	Region::type_t tile = 0;
	for (register int i = 0; i < REGION_WIDTH; ++i) {
		for (register int j = 0; j < REGION_HEIGHT; ++j) {
			for (register int k = 0; k < REGION_DEPTH; ++k) {
				tile = region->GetTile(i, j, k);
				//0 is invisible
				if (tile == 0) continue;
				clip.x = (tile-1) % countX * clip.h;
				clip.y = (tile-1) / countX * clip.w;
				//TODO: (2) raw rendering; improve preformance
				Image::DrawTo(renderer,
					(region->GetX() + i) * clip.w - camX,
					(region->GetY() + j) * clip.h - camY,
					scaleX, scaleY);
			}
		}
	}
}
