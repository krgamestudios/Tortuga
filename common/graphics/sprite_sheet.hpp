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
#pragma once

#include "image.hpp"

class SpriteSheet : public Image {
public:
	SpriteSheet() = default;
	SpriteSheet(SpriteSheet const& rhs) { *this = rhs; }
	SpriteSheet(SpriteSheet&& rhs) { *this = std::move(rhs); }
	SpriteSheet(SDL_Renderer* r, std::string fname, Uint16 cx, Uint16 cy)
		{ Load(r, fname, cx, cy); }
	SpriteSheet(SDL_Renderer* r, Uint16 w, Uint16 h, Uint16 cx, Uint16 cy)
		{ Create(r, w, h, cx, cy); }
	SpriteSheet(SDL_Texture* p, Uint16 cx, Uint16 cy)
		{ SetTexture(p, cx, cy); }
	~SpriteSheet() = default;

	SpriteSheet& operator=(SpriteSheet const&);
	SpriteSheet& operator=(SpriteSheet&&);

	void Update(double delta);

	SDL_Texture* Load(SDL_Renderer*, std::string fname, Uint16 cx, Uint16 cy);
	SDL_Texture* Create(SDL_Renderer*, Uint16 w, Uint16 h, Uint16 cx, Uint16 cy);
	SDL_Texture* SetTexture(SDL_Texture*, Uint16 cx, Uint16 cy);
	void Free() override;

	Uint16 SetCountX(Uint16);
	Uint16 SetCountY(Uint16);
	Uint16 SetIndexX(Uint16);
	Uint16 SetIndexY(Uint16);

	Uint16 GetCountX() const { return countX; }
	Uint16 GetCountY() const { return countY; }
	Uint16 GetIndexX() const { return indexX; }
	Uint16 GetIndexY() const { return indexY; }

	double SetDelay(double d);
	double GetDelay() const { return delay; }

private:
	Uint16 countX = 0, countY = 0, indexX = 0, indexY = 0;
	double delay = 0.0, tick = 0.0;

	//disable access
	using Image::Load;
	using Image::Create;
	using Image::SetTexture;
};