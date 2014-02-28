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
#ifndef TILESHEET_HPP_
#define TILESHEET_HPP_

#include "image.hpp"

#include <string>

class TileSheet {
public:
	TileSheet() = default;
	TileSheet(std::string f, int x, int y) { Load(f, x, y); }
	~TileSheet() = default;

	void Load(std::string fname, int XCount, int YCount);
	void Unload();

	void DrawTo(SDL_Surface* const dest, int x, int y, int tile);

	//accessors
	Image* GetImage() { return &image; }
	int GetXCount() { return XCount; }
	int GetYCount() { return YCount; }
	int GetTileW() { return image.GetClipW(); }
	int GetTileH() { return image.GetClipH(); }
private:
	Image image;
	int XCount = 0, YCount = 0;
};

#endif
