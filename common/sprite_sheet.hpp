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

class SpriteSheet : protected Image {
public:
	SpriteSheet(SDL_Surface*, Uint16 w, Uint16 h);
	virtual ~SpriteSheet() {}

	void Update(int delta);

	void SetSurface(SDL_Surface*, Uint16 w, Uint16 h);

	using Image::GetSurface;
	using Image::DrawTo;

	//TODO: these aren't regulated; be careful
	int SetWidth(int i) { return SetClipW(i); };
	int SetHeight(int i) { return SetClipH(i); };
	int SetFrames(int i) { currentFrame = 0; return maxFrames = i; };
	int SetStrips(int i) { currentStrip = 0; return maxStrips = i; };

	int GetWidth() const { return GetClipW(); }
	int GetHeight() const { return GetClipH(); }
	int GetFrames() const { return maxFrames; }
	int GetStrips() const { return maxStrips; }

	int SetCurrentFrame(int i) { return currentFrame = i; };
	int SetCurrentStrip(int i) { return currentStrip = i; };
	int SetInterval(int i) { return interval = i; }

	int GetCurrentFrame() const { return currentFrame; };
	int GetCurrentStrip() const { return currentStrip; };
	int GetInterval() const { return interval; };
private:
	int currentFrame, maxFrames;
	int currentStrip, maxStrips;
	int interval, ticks;
};

#endif
