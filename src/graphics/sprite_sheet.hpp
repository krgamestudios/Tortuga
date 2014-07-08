/* Copyright: (c) Kayne Ruse 2013, 2014
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
#ifndef SPRITESHEET_HPP_
#define SPRITESHEET_HPP_

#include "image.hpp"

class SpriteSheet {
public:
	SpriteSheet() = default;
	SpriteSheet(std::string fname, Uint16 xCellCount, Uint16 yCellCount) { LoadSurface(fname, xCellCount, yCellCount); }
	SpriteSheet(SDL_Surface* surface, Uint16 xCellCount, Uint16 yCellCount) { SetSurface(surface, xCellCount, yCellCount); }
	~SpriteSheet() { FreeSurface(); };

	void Update(double delta);

	SDL_Surface* LoadSurface(std::string fname, Uint16 xCellCount, Uint16 yCellCount);
	SDL_Surface* SetSurface(SDL_Surface* surface, Uint16 xCellCount, Uint16 yCellCount);
	SDL_Surface* GetSurface() { return image.GetSurface(); }
	void FreeSurface();

	void DrawTo(SDL_Surface* const dest, Sint16 x, Sint16 y) { image.DrawTo(dest, x, y); }

	//accessors and mutators
	Image* GetImage() { return &image; } //OO breaker

	Uint16 SetXCount(Uint16);
	Uint16 SetYCount(Uint16);
	Uint16 SetXIndex(Uint16);
	Uint16 SetYIndex(Uint16);

	Uint16 GetXCount() const { return xCount; }
	Uint16 GetYCount() const { return yCount; }
	Uint16 GetXIndex() const { return xIndex; }
	Uint16 GetYIndex() const { return yIndex; }

	double SetDelay(double d);
	double GetDelay() const { return delay; }

private:
	Image image;
	Uint16 xCount = 0, yCount = 0; //number of cells
	Uint16 xIndex = 0, yIndex = 0; //current cell being drawn
	double delay = 0.0, tick = 0.0;
};

#endif
