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
#pragma once

#include "region.hpp"

#include "image.hpp"

#include <string>

class TileSheet : public Image {
public:
	TileSheet() = default;
	TileSheet(TileSheet const& rhs) { *this = rhs; }
	TileSheet(TileSheet&& rhs) { *this = std::move(rhs); }
	TileSheet(SDL_Renderer* r, std::string fn, int tw, int th) { Load(r, fn, tw, th); }
	TileSheet(SDL_Texture* p, int tw, int th) { SetTexture(p, tw, th); }
	~TileSheet() = default;

	TileSheet& operator=(TileSheet const&);
	TileSheet& operator=(TileSheet&&);

	void Load(SDL_Renderer*, std::string fname, int tileWidth, int tileHeight);
	SDL_Texture* SetTexture(SDL_Texture*, int tileWidth, int tileHeight);
	void Free() override;

	void DrawLayerTo(SDL_Renderer* const renderer, Region* const region, int layer, int camX, int camY, double scaleX = 1.0, double scaleY = 1.0);
	void DrawRegionTo(SDL_Renderer* const renderer, Region* const region, int camX, int camY, double scaleX = 1.0, double scaleY = 1.0);

	//accessors
	//DOCS: Reuse Image::clip for tile sizes
	int GetCountX() { return countX; }
	int GetCountY() { return countY; }
	int GetTileW() { return clip.w; }
	int GetTileH() { return clip.h; }

protected:
	int countX = 0, countY = 0;

	using Image::Load;
	using Image::Create;
	using Image::SetTexture;
	using Image::SetClip;
	using Image::SetClipX;
	using Image::SetClipY;
	using Image::SetClipW;
	using Image::SetClipH;
};
