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

#include "SDL2/SDL.h"

#include <string>

class Image {
public:
	Image() = default;
	Image(Image const& rhs) { *this = rhs; }
	Image(Image&& rhs) { *this = std::move(rhs); }
	Image(SDL_Renderer* r, std::string fname) { Load(r, fname); }
	Image(SDL_Renderer* r, Uint16 w, Uint16 h) { Create(r, w, h); }
	Image(SDL_Texture* p) { SetTexture(p); }
	virtual ~Image() { Free(); }

	Image& operator=(Image const&);
	Image& operator=(Image&&);

	SDL_Texture* Load(SDL_Renderer* renderer, std::string fname);
	SDL_Texture* Create(SDL_Renderer* renderer, Uint16 w, Uint16 h);
	SDL_Texture* CopyTexture(SDL_Renderer* renderer, SDL_Texture* ptr);
	SDL_Texture* SetTexture(SDL_Texture*);
	SDL_Texture* GetTexture() const;
	virtual void Free();

	void DrawTo(SDL_Renderer* const, Sint16 x, Sint16 y, double scaleX = 1.0, double scaleY = 1.0);

	void SetAlpha(Uint8 a);
	Uint8 GetAlpha();

	//Clip handlers
	SDL_Rect SetClip(SDL_Rect r) { return clip = r; }
	SDL_Rect GetClip() const { return clip; }

	Sint16 SetClipX(Sint16 x) { return clip.x = x; }
	Sint16 SetClipY(Sint16 y) { return clip.y = y; }
	Uint16 SetClipW(Uint16 w) { return clip.w = w; }
	Uint16 SetClipH(Uint16 h) { return clip.h = h; }

	Sint16 GetClipX() const { return clip.x; }
	Sint16 GetClipY() const { return clip.y; }
	Uint16 GetClipW() const { return clip.w; }
	Uint16 GetClipH() const { return clip.h; }

	bool GetLocal() const { return local; }

protected:
	SDL_Texture* texture = nullptr;
	SDL_Rect clip = {0, 0, 0, 0};
	bool local = false;
};