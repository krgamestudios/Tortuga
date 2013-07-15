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
#include "image.hpp"

#include <stdexcept>

SDL_Surface* Image::LoadSurface(std::string fname) {
	SDL_Surface* p = SDL_LoadBMP(fname.c_str());
	if (!p) {
		throw(std::runtime_error(std::string() + "Failed to load file: " + fname));
	}
	surface = p;
	clip = {0, 0, (Uint16)surface->w, (Uint16)surface->h};
	local = true;
	return surface;
}

void Image::FreeSurface() {
	if (local) {
		SDL_FreeSurface(surface);
		local = false;
	}
	surface = nullptr;
	clip = {0, 0, 0, 0};
}

SDL_Surface* Image::SetSurface(SDL_Surface* p) {
	if (!p) {
		throw(std::invalid_argument("No surface pointer provided"));
	}
	surface = p;
	clip = {0, 0, (Uint16)surface->w, (Uint16)surface->h};
	local = false;
	return surface;
}

void Image::DrawTo(SDL_Surface* dest, Sint16 x, Sint16 y) {
	if (!surface) {
		throw(std::logic_error("No image surface to draw"));
	}
	SDL_Rect sclip = clip, dclip = {x,y};
	SDL_BlitSurface(surface, &sclip, dest, &dclip);
}
