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
#include "raster_font.hpp"

#include <stdexcept>

void RasterFont::DrawStringTo(std::string s, SDL_Surface* const dest, Sint16 x, Sint16 y) {
	if (!image.GetSurface()) {
		throw(std::runtime_error("RasterFont not loaded"));
	}
	const Uint16 w = image.GetClipW();
	const Uint16 h = image.GetClipH();
	for (int i = 0; i < s.size(); i++) {
		image.SetClipX(s[i] % w * w);
		image.SetClipY(s[i] / h * h);
		image.DrawTo(dest, x + i * w, y);
	}
}

SDL_Surface* RasterFont::SetSurface(SDL_Surface* p) {
	if (image.SetSurface(p)) {
		image.SetClipW(image.GetSurface()->w/16);
		image.SetClipH(image.GetSurface()->h/16);
	}
	return image.GetSurface();
}
