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
#include "sprite_sheet.hpp"

SpriteSheet::SpriteSheet(SDL_Surface* s, Uint16 w, Uint16 h)
	: Image(s, {0, 0, w, h})
{
	currentFrame = 0; maxFrames = GetSurface()->w / GetClipW();
	currentStrip = 0; maxStrips = GetSurface()->h / GetClipH();
	interval = ticks = 0;
}

void SpriteSheet::Update(int delta) {
	if (interval && (ticks += delta) > interval) {
		if (++currentFrame >= maxFrames) {
			currentFrame = 0;
		}
		ticks = 0;
	}
	SetClipX(currentFrame * GetClipW());
	SetClipY(currentStrip * GetClipH());
}

void SpriteSheet::SetSurface(SDL_Surface* s, Uint16 w, Uint16 h) {
	Image::SetSurface(s, {0, 0, w, h});
	currentFrame = 0; maxFrames = GetSurface()->w / GetClipW();
	currentStrip = 0; maxStrips = GetSurface()->h / GetClipH();
	interval = ticks = 0;
}