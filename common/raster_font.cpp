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
#include "raster_font.hpp"

void RasterFont::DrawStringTo(std::string s, SDL_Surface* const dest, Sint16 x, Sint16 y) {
	if (!surface) {
		return;
	}
	//character size won't change here
	const Sint16 w = surface->w/16;
	const Sint16 h = surface->h/16;
	SDL_Rect sclip, dclip = {x,y,0,0};
	for (auto c : s) {
		//TODO: inefficient
		sclip = {Sint16(c%w*w), Sint16(c/h*h), clip.w, clip.h};
//		sclip.x = c % w;
//		sclip.h = c / h;
//		sclip.w = clip.w;
//		sclip.h = clip.h;
		SDL_BlitSurface(surface, &sclip, dest, &dclip);
		dclip.x += w;
	}
}

SDL_Surface* RasterFont::SetSurface(SDL_Surface* p) {
	surface = p;
	if (!surface) {
		clip = {0, 0, 0, 0};
	}
	else {
		clip = {0, 0, Uint16(surface->w/16), Uint16(surface->h/16)};
	}
	return surface;
}
