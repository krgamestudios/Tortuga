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
#ifndef RASTERFONT_HPP_
#define RASTERFONT_HPP_

#include "image.hpp"

class RasterFont {
public:
	RasterFont() = default;
	RasterFont(RasterFont const& rhs) { *this = rhs; }
	RasterFont(RasterFont&& rhs) { *this = std::move(rhs); }
	RasterFont(std::string fname) { LoadSurface(fname); }
	RasterFont(SDL_Surface* p) { SetSurface(p); }
	~RasterFont() = default;

	RasterFont& operator=(RasterFont const& rhs) { image = rhs.image; }
	RasterFont& operator=(RasterFont&& rhs) { image = std::move(rhs.image); }

	void DrawStringTo(std::string, SDL_Surface* const, Sint16 x, Sint16 y);

	//Accessors and Mutators
	SDL_Surface* LoadSurface(std::string);
	SDL_Surface* SetSurface(SDL_Surface*);
	SDL_Surface* GetSurface() const { return image.GetSurface(); }
	void FreeSurface() { image.FreeSurface(); }

	Uint16 GetCharW() const { return image.GetClipW(); }
	Uint16 GetCharH() const { return image.GetClipH(); }

private:
	Image image;
};

#endif
