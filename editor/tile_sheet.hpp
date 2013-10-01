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
#ifndef TILESHEET_HPP_
#define TILESHEET_HPP_

#include "image.hpp"

#include <string>

/* The TileSheet class is used for drawing tiles of the map to the screen.
 * This class also tracks the range of the tile images.
*/
class TileSheet {
public:
	TileSheet() = default;
	~TileSheet() = default;

	//these load/set functions need to be followed by bookkeeping code
	//w & h are the width & height of individual tiles
	SDL_Surface* LoadSurface(std::string fname, Uint16 w, Uint16 h);
	SDL_Surface* SetSurface(SDL_Surface* const, Uint16 w, Uint16 h);
	SDL_Surface* GetSurface();
	void FreeSurface();

	void DrawTo(SDL_Surface* const dest, int x, int y, int tileIndex);

	bool InRange(int i) { return i >= begin && i < end; }

	//accessors and mutators
	Image* GetImage() { return &image; }

	std::string SetName(std::string s) { return name = s; }
	std::string GetName() const { return name; }

	int GetTotalCount() const { return totalCount; }
	int GetXCount() const { return xCount; }
	int GetYCount() const { return yCount; }

	int SetBegin(int i) { return begin = i; }
	int SetEnd(int i) { return end = i; }

	int GetBegin() const { return begin; }
	int GetEnd() const { return end; }
private:
	Image image;
	std::string name;

	//these are generated and used by internal processes
	int totalCount = 0, xCount = 0, yCount = 0;
	int begin = -1, end = -1;
};

#endif
