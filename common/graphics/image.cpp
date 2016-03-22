/* Copyright: (c) Kayne Ruse 2013-2016
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

#include "SDL2/SDL_image.h"

#include <sstream>
#include <stdexcept>

Image& Image::operator=(Image const& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	Free();

	//Copy the other Image's stuff
	texture = rhs.texture;
	clip = rhs.clip;
	local = false;
}

Image& Image::operator=(Image&& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	Free();

	//Steal the other Image's stuff
	texture = rhs.texture;
	clip = rhs.clip;
	local = rhs.local;

	rhs.texture = nullptr;
	rhs.clip = {0, 0, 0, 0};
	rhs.local = false;
}

SDL_Texture* Image::Load(SDL_Renderer* renderer, std::string fname) {
	Free();

	//load the file into a surface
	SDL_Surface* surface = IMG_Load(fname.c_str());
	if (!surface) {
		std::ostringstream msg;
		msg << "Failed to load an image file: " << fname;
		msg << "; " << IMG_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//create a texture from this surface
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		std::ostringstream msg;
		msg << "Failed to convert a newly loaded image file: " << fname;
		msg << "; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//set the metadata
	clip.x = 0;
	clip.y = 0;
	if (SDL_QueryTexture(texture, nullptr, nullptr, &clip.w, &clip.h)) {
		std::ostringstream msg;
		msg << "Failed to record metadata for a newly loaded image file: " << fname;
		msg << "; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
	local = true;

	//free the surface & return
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture* Image::Create(SDL_Renderer* renderer, Uint16 w, Uint16 h, SDL_Color blank) {
	Free();

	//make the texture
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		w, h);

	//check
	if (!texture) {
		std::ostringstream msg;
		msg << "Failed to create a texture; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//set the metadata
	clip.x = 0;
	clip.y = 0;
	if (SDL_QueryTexture(texture, nullptr, nullptr, &clip.w, &clip.h)) {
		std::ostringstream msg;
		msg << "Failed to record metadata for a newly created image";
		msg << "; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
	local = true;

	//blank (black) texture
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, blank.r, blank.g, blank.b, blank.a);
	SDL_RenderFillRect(renderer, nullptr);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetRenderTarget(renderer, nullptr);

	return texture;
}

SDL_Texture* Image::CopyTexture(SDL_Renderer* renderer, SDL_Texture* ptr) {
	Free();
	int w = 0, h = 0;

	//get the info
	SDL_QueryTexture(ptr, nullptr, nullptr, &w, &h);

	//create a texture of (w, h) size (also sets the metadata)
	Create(renderer, w, h);

	//copy the argument texture to the local texture
	SDL_SetRenderTarget(renderer, texture);
	SDL_RenderCopy(renderer, ptr, nullptr, nullptr);
	SDL_SetRenderTarget(renderer, nullptr);

	//return the local texture
	return texture;
}

SDL_Texture* Image::SetTexture(SDL_Texture* ptr) {
	Free();

	texture = ptr;

	//set the metadata
	clip.x = 0;
	clip.y = 0;
	if (SDL_QueryTexture(texture, nullptr, nullptr, &clip.w, &clip.h)) {
		std::ostringstream msg;
		msg << "Failed to record metadata for a newly image image";
		msg << "; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
	local = false;

	return texture;
}

SDL_Texture* Image::GetTexture() const {
	return texture;
}

void Image::Free() {
	if (local) {
		SDL_DestroyTexture(texture);
		local = false;
	}
	texture = nullptr;
	clip = {0, 0, 0, 0};
}

void Image::DrawTo(SDL_Renderer* const renderer, Sint16 x, Sint16 y, double scaleX, double scaleY) {
	if (!texture) {
		throw(std::logic_error("No image texture to draw"));
	}
	SDL_Rect sclip = clip;
	SDL_Rect dclip = {x, y, Uint16(clip.w * scaleX), Uint16(clip.h * scaleY)};
	SDL_RenderCopy(renderer, texture, &sclip, &dclip);
}

void Image::SetAlpha(Uint8 a) {
	if (SDL_SetTextureAlphaMod(texture, a)) {
		std::ostringstream msg;
		msg << "Failed to set alpha; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
}

Uint8 Image::GetAlpha() {
	Uint8 ret = 0;
	if (SDL_GetTextureAlphaMod(texture, &ret)) {
		std::ostringstream msg;
		msg << "Failed to get alpha; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
	return ret;
}