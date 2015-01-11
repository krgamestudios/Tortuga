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
#ifndef TILESHEET_HPP_
#define TILESHEET_HPP_

#include "region.hpp"

#include "image.hpp"

#include <string>

class TileSheet {
public:
	TileSheet() = default;
	TileSheet(std::string f, int w, int h) { Load(f, w, h); }
	~TileSheet() = default;

	void Load(std::string fname, int tileWidth, int tileHeight);
	void Unload();

	void DrawTileTo(SDL_Surface* const dest, int x, int y, Region::type_t tile);
	void DrawRegionTo(SDL_Surface* const dest, Region* const region, int camX, int camY);

	//accessors
	Image* GetImage() { return &image; }
	int GetXCount() { return xCount; }
	int GetYCount() { return yCount; }
	int GetTileW() { return image.GetClipW(); }
	int GetTileH() { return image.GetClipH(); }
private:
	Image image;
	int xCount = 0, yCount = 0;
};

#endif
