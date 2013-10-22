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
#include "tile_sheet.hpp"

#include <stdexcept>

SDL_Surface* TileSheet::LoadSurface(std::string fname, Uint16 w, Uint16 h) {
	//setup the image
	image.LoadSurface(fname);
	image.SetClipW(w);
	image.SetClipH(h);

	//get the tile counts
	xCount = image.GetSurface()->w / w;
	yCount = image.GetSurface()->h / h;
	totalCount = xCount * yCount;

	//set begin & end (usually temporary)
	begin = 0;
	end = totalCount;
}

SDL_Surface* TileSheet::GetSurface() {
	return image.GetSurface();
}

void TileSheet::FreeSurface() {
	image.FreeSurface();
	totalCount = xCount = yCount = 0;
	begin = end = -1;
}

void TileSheet::DrawTo(SDL_Surface* const dest, int x, int y, int tileIndex) {
	if (!InRange(tileIndex)) {
		throw(std::invalid_argument("Tile index out of range"));
	}
	Sint16 clipX = (tileIndex-begin) % xCount * image.GetClipW();
	Sint16 clipY = (tileIndex-begin) / xCount * image.GetClipH();

	image.SetClipX(clipX);
	image.SetClipY(clipY);

	image.DrawTo(dest, x, y);
}
