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
#ifndef SPRITESHEET_HPP_
#define SPRITESHEET_HPP_

#include "image.hpp"

#include "SDL/SDL.h"

class SpriteSheet {
public:
	SpriteSheet() = default;
	SpriteSheet(SDL_Surface*, Uint16 w, Uint16 h);
	~SpriteSheet() = default;

	void Update(double delta);

	SDL_Surface* SetSurface(SDL_Surface* const, Uint16 w, Uint16 h);
	SDL_Surface* GetSurface() const {
		return image.GetSurface();
	}

	void DrawTo(SDL_Surface* const dest, Sint16 x, Sint16 y) {
		image.DrawTo(dest, x, y);
	}

	//Accessors and Mutators
	double SetInterval(double i) { return interval = i; }
	double GetInterval() const { return interval; }

	int SetCurrentFrame(int i) { return currentFrame = i; }
	int SetCurrentStrip(int i) { return currentStrip = i; }

	Uint16 GetFrameWidth() const { return image.GetClipW(); }
	Uint16 GetFrameHeight() const { return image.GetClipH(); }
	int GetCurrentFrame() const { return currentFrame; };
	int GetCurrentStrip() const { return currentStrip; };
	int GetMaxFrames() const { return maxFrames; }
	int GetMaxStrips() const { return maxStrips; }
private:
	Image image;
	int currentFrame = 0, maxFrames = 0;
	int currentStrip = 0, maxStrips = 0;
	 double interval = 0, ticks = 0;
};

#endif
