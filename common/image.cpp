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
#include <sstream>

Image& Image::operator=(Image const& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	FreeSurface();

	//Copy the other Image's stuff
	surface = rhs.surface;
	clip = rhs.clip;
	local = false;
}

Image& Image::operator=(Image&& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	FreeSurface();

	//Steal the other Image's stuff
	surface = rhs.surface;
	clip = rhs.clip;
	local = rhs.local;

	rhs.surface = nullptr;
	rhs.clip = {0, 0, 0, 0};
	rhs.local = false;
}

SDL_Surface* Image::LoadSurface(std::string fname) {
	FreeSurface();
	SDL_Surface* p = SDL_LoadBMP(fname.c_str());
	if (!p) {
		std::ostringstream os;
		os << "Failed to load file: " << fname;
		throw(std::runtime_error(os.str()));
	}
	surface = p;
	clip = {0, 0, (Uint16)surface->w, (Uint16)surface->h};
	local = true;
	SetTransparentColor(255, 0, 255); //default
	return surface;
}

SDL_Surface* Image::CreateSurface(Uint16 w, Uint16 h) {
	FreeSurface();
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
	SDL_Surface* p = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, rmask, gmask, bmask, amask);
	if (!p) {
		throw(std::runtime_error("Failed to create Image surface"));
	}
	surface = p;
	clip = {0, 0, (Uint16)surface->w, (Uint16)surface->h};
	local = true;
	SetTransparentColor(255, 0, 255); //default
	return surface;
}

SDL_Surface* Image::SetSurface(SDL_Surface* p) {
	FreeSurface();
	if (!p) {
		throw(std::invalid_argument("No surface pointer provided"));
	}
	surface = p;
	clip = {0, 0, (Uint16)surface->w, (Uint16)surface->h};
	local = false;
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

void Image::DrawTo(SDL_Surface* dest, Sint16 x, Sint16 y) {
	if (!surface) {
		throw(std::logic_error("No image surface to draw"));
	}
	SDL_Rect sclip = clip, dclip = {x,y};
	SDL_BlitSurface(surface, &sclip, dest, &dclip);
}

void Image::SetTransparentColor(Uint8 r, Uint8 g, Uint8 b) {
	if (!surface) {
		throw(std::logic_error("Failed to set the transparent color"));
	}
	if (!local) {
		throw(std::logic_error("Cannot set the transparent color of a non-local surface"));
	}
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, r, g, b));
}

void Image::ClearTransparentColor() {
	if (!surface) {
		throw(std::logic_error("Failed to clear the transparent color"));
	}
	if (!local) {
		throw(std::logic_error("Cannot clear the transparent color of a non-local surface"));
	}
	SDL_SetColorKey(surface, 0, 0);
}
