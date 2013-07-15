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

void SpriteSheet::Update(double delta) {
	if (delay && (ticks += delta) >= delay) {
		if (++currentFrame >= maxFrames) {
			currentFrame = 0;
		}
		ticks = 0;
	}
	image.SetClipX(currentFrame * image.GetClipW());
	image.SetClipY(currentStrip * image.GetClipH());
}

SDL_Surface* SpriteSheet::SetSurface(SDL_Surface* const s, Uint16 w, Uint16 h) {
	image.SetSurface(s);
	image.SetClip({0, 0, w, h});
	currentFrame = 0; maxFrames = image.GetSurface()->w / image.GetClipW();
	currentStrip = 0; maxStrips = image.GetSurface()->h / image.GetClipH();
	delay = ticks = 0;
}
