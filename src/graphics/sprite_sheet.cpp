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
#include "sprite_sheet.hpp"

#include <stdexcept>
#include <sstream>

void SpriteSheet::Update(double delta) {
	if (delay && (tick += delta) >= delay) {
		if (++xIndex >= xCount) {
			xIndex = 0;
		}
		tick = 0;
	}
	image.SetClipX(xIndex * image.GetClipW());
	image.SetClipY(yIndex * image.GetClipH());
}

SDL_Surface* SpriteSheet::LoadSurface(std::string fname, Uint16 xCellCount, Uint16 yCellCount) {
	image.LoadSurface(fname);

	xCount = xCellCount;
	yCount = yCellCount;

	image.SetClipW(image.GetSurface()->w / xCount);
	image.SetClipH(image.GetSurface()->h / yCount);

	xIndex = yIndex = 0;
	delay = tick = 0.0;
}

SDL_Surface* SpriteSheet::SetSurface(SDL_Surface* surface, Uint16 xCellCount, Uint16 yCellCount) {
	image.SetSurface(surface);

	xCount = xCellCount;
	yCount = yCellCount;

	image.SetClipW(image.GetSurface()->w / xCount);
	image.SetClipH(image.GetSurface()->h / yCount);

	xIndex = yIndex = 0;
	delay = tick = 0.0;
}

void SpriteSheet::FreeSurface() {
	image.FreeSurface();
	xCount = yCount = 0;
	xIndex = yIndex = 0;
	delay = tick = 0.0;
}

Uint16 SpriteSheet::SetXCount(Uint16 i) {
	xIndex = 0;
	return xCount = i;
}

Uint16 SpriteSheet::SetYCount(Uint16 i) {
	yIndex = 0;
	return yCount = i;
}

Uint16 SpriteSheet::SetXIndex(Uint16 i) {
	if (i > xCount) {
		std::ostringstream os;
		os << "Cannot set x index to " << i;
		throw(std::invalid_argument(os.str()));
	}
	return xIndex = i;
}

Uint16 SpriteSheet::SetYIndex(Uint16 i) {
	if (i > yCount) {
		std::ostringstream os;
		os << "Cannot set y index to " << i;
		throw(std::invalid_argument(os.str()));
	}
	return yIndex = i;
}

double SpriteSheet::SetDelay(double d) {
	tick = 0;
	return delay = d;
}
