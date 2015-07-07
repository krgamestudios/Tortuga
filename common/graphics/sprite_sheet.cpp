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
#include "sprite_sheet.hpp"

#include <sstream>
#include <stdexcept>

SpriteSheet& SpriteSheet::operator=(SpriteSheet const& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	Free();

	//Copy the other SpriteSheet's stuff
	texture = rhs.texture;
	clip = rhs.clip;
	local = false;
	countX = rhs.countX;
	countY = rhs.countY;
	indexX = rhs.indexX;
	indexY = rhs.indexY;
	delay = rhs.delay;
	tick = rhs.tick;
}

SpriteSheet& SpriteSheet::operator=(SpriteSheet&& rhs) {
	//don't screw yourself
	if (this == &rhs) {
		return *this;
	}

	Free();

	//Steal the other SpriteSheet's stuff
	texture = rhs.texture;
	clip = rhs.clip;
	local = rhs.local;
	countX = rhs.countX;
	countY = rhs.countY;
	indexX = rhs.indexX;
	indexY = rhs.indexY;
	delay = rhs.delay;
	tick = rhs.tick;

	rhs.texture = nullptr;
	rhs.clip = {0, 0, 0, 0};
	rhs.local = false;
	rhs.countX = 0;
	rhs.countY = 0;
	rhs.indexX = 0;
	rhs.indexY = 0;
	rhs.delay = 0.0;
	rhs.tick = 0.0;
}

void SpriteSheet::Update(double delta) {
	//if the delay has passed
	if (delay && (tick += delta) >= delay) {
		//if the index is out of bounds
		if (++indexX >= countX) {
			indexX = 0;
		}
		tick = 0;
	}
	//modify area drawn
	clip.x = indexX * clip.w;
	clip.y = indexX * clip.y;
}

SDL_Texture* SpriteSheet::Load(SDL_Renderer* r, std::string fname, Uint16 cx, Uint16 cy) {
	//call the base function
	Image::Load(r, fname);

	//set the metadata
	countX = cx;
	countY = cy;

	//assume clip.x and clip.y were set to the size of the texture
	//reduce the w & h to the size of one cell
	clip.w = clip.w / countX;
	clip.h = clip.h / countY;

	indexX = indexY = 0;
	delay = tick = 0.0;

	return texture;
}

SDL_Texture* SpriteSheet::Create(SDL_Renderer* r, Uint16 w, Uint16 h, Uint16 cx, Uint16 cy) {
	//call the base function
	Image::Create(r, w, h);

	//set the metadata
	countX = cx;
	countY = cy;

	//assume clip.x and clip.y were set to the size of the texture
	//reduce the w & h to the size of one cell
	clip.w = clip.w / countX;
	clip.h = clip.h / countY;

	indexX = indexY = 0;
	delay = tick = 0.0;

	return texture;
}

SDL_Texture* SpriteSheet::SetTexture(SDL_Texture* ptr, Uint16 cx, Uint16 cy) {
	//call the base function
	Image::SetTexture(ptr);

	//set the metadata
	countX = cx;
	countY = cy;

	//assume clip.x and clip.y were set to the size of the texture
	//reduce the w & h to the size of one cell
	clip.w = clip.w / countX;
	clip.h = clip.h / countY;

	indexX = indexY = 0;
	delay = tick = 0.0;

	return texture;
}

void SpriteSheet::Free() {
	Image::Free();
	countX = countY = 0;
	indexX = indexY = 0;
	delay = tick = 0.0;
}

Uint16 SpriteSheet::SetCountX(Uint16 i) {
	indexX = 0;
	return countX = i;
}

Uint16 SpriteSheet::SetCountY(Uint16 i) {
	indexY = 0;
	return countY = i;
}

Uint16 SpriteSheet::SetIndexX(Uint16 i) {
	if (i > countX) {
		std::ostringstream msg;
		msg << "Cannot set index 'x' to " << i;
		throw(std::out_of_range(msg.str()));
	}
	return indexX = i;
}

Uint16 SpriteSheet::SetIndexY(Uint16 i) {
	if (i > countY) {
		std::ostringstream msg;
		msg << "Cannot set index 'y' to " << i;
		throw(std::invalid_argument(msg.str()));
	}
	return indexY = i;
}

double SpriteSheet::SetDelay(double d) {
	tick = 0;
	return delay = d;
}