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

void TileSheet::DrawTo(SDL_Surface* const dest, int x, int y, int tile) {
	image.SetClipX(tile % XCount * image.GetClipW());
	image.SetClipY(tile / XCount * image.GetClipH());
	image.DrawTo(dest, x, y);
}
