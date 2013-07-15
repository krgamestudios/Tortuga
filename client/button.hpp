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
#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include "image.hpp"
#include "raster_font.hpp"

#include <string>

//3-phases, no toggle, centred text
class Button {
public:
	enum class State {
		NORMAL, HOVER, PRESSED
	};

	Button() = default;
	Button(Sint16 x, Sint16 y, SDL_Surface* bg, SDL_Surface* fg, std::string t = "") { Setup(x, y, bg, fg, t); }

	void Setup(Sint16 x, Sint16 y, SDL_Surface* bg, SDL_Surface* fg, std::string text = "");

	//return the current state
	State MouseMotion(SDL_MouseMotionEvent const&);
	State MouseButtonDown(SDL_MouseButtonEvent const&);
	State MouseButtonUp(SDL_MouseButtonEvent const&);
	State GetState() const { return state; }

	//yet another draw function
	void DrawTo(SDL_Surface* const);

	//simple accessors and mutators
	Sint16 SetX(Sint16 i) { return x = i; }
	Sint16 SetY(Sint16 i) { return y = i; }
	Sint16 GetX() const { return x; }
	Sint16 GetY() const { return y; }

	void SetSurfaces(SDL_Surface* bg, SDL_Surface* fg);

	std::string SetText(std::string t);
	std::string GetText() const { return text; }

	//raw access, be careful
	Image* GetImage() { return &image; }
	RasterFont* GetFont() { return &font; }

	//debug
	Sint16 GetTextX() const { return textX; }
	Sint16 GetTextY() const { return textY; }
private:
	State CalcState(Sint16 x, Sint16 y, bool leftPressed);

	Sint16 x = 0, y = 0;
	Sint16 textX = 0, textY = 0; //prevent recalc every loop
	Image image;
	RasterFont font;
	State state = State::NORMAL;
	std::string text;
};

#endif
