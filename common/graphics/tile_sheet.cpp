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
#include "tile_sheet.hpp"

void TileSheet::Load(std::string fname, int xc, int yc) {
	XCount = xc;
	YCount = yc;
	image.LoadSurface(fname);
	image.SetClipW(image.GetClipW()/XCount);
	image.SetClipH(image.GetClipH()/YCount);
}

void TileSheet::Unload() {
	image.FreeSurface();
	XCount = YCount = 0;
}

void TileSheet::DrawTo(SDL_Surface* const dest, int x, int y, Region::type_t tile) {
	//0 is invisible
	if (tile == 0) return;
	image.SetClipX((tile-1) % XCount * image.GetClipW());
	image.SetClipY((tile-1) / XCount * image.GetClipH());
	image.DrawTo(dest, x, y);
}

void TileSheet::DrawRegionTo(SDL_Surface* const dest, Region* const region, int camX, int camY) {
	Region::type_t tile = 0;
	for (register int i = 0; i < REGION_WIDTH; ++i) {
		for (register int j = 0; j < REGION_HEIGHT; ++j) {
			for (register int k = 0; k < REGION_DEPTH; ++k) {
				tile = region->GetTile(i, j, k);
				//0 is invisible
				if (tile == 0) continue;
				image.SetClipX((tile-1) % XCount * image.GetClipW());
				image.SetClipY((tile-1) / XCount * image.GetClipH());
				image.DrawTo(dest,
					region->GetX() + i * image.GetClipW() - camX,
					region->GetY() + j * image.GetClipH() - camY);
			}
		}
	}
}