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
#include "raster_font.hpp"

#include <string>

/* 3-phases, no toggle, centred text
 * This class uses the size of the provided image as its bounds. Also,
 * The provided image should be formatted correctly.
 *
 * The button's image should be divided into 3 sections virtucally,
 * which act as the different button images. The clip width & height of the
 * Image should be set manually, and the height should be 1/3 of the total
 * graphical data.
*/
class Button {
public:
	enum State {
		NORMAL = 0, HOVER = 1, PRESSED = 2
	};

	Button() = default;
	~Button() = default;

	//handle input
	State MouseMotion(SDL_MouseMotionEvent const&);
	State MouseButtonDown(SDL_MouseButtonEvent const&);
	State MouseButtonUp(SDL_MouseButtonEvent const&);

	//yet another draw function
	void DrawTo(SDL_Surface* const);

	//accessors and mutators
	Image* SetImage(Image* const ptr) { return image = ptr; }
	Image* GetImage() { return image; }
	RasterFont* SetFont(RasterFont* const ptr) { return font = ptr; }
	RasterFont* GetFont() { return font; }

	Sint16 SetX(Sint16 i) { return x = i; }
	Sint16 SetY(Sint16 i) { return y = i; }
	Sint16 GetX() const { return x; }
	Sint16 GetY() const { return y; }

	Sint16 SetTextX(Sint16 i) { return textX = i; }
	Sint16 SetTextY(Sint16 i) { return textY = i; }
	Sint16 GetTextX() const { return textX; }
	Sint16 GetTextY() const { return textY; }

	State SetState(State s) { return state = s; }
	State GetState() const { return state; }

	std::string SetText(std::string);
	std::string GetText() const { return text; }

private:
	State CalcState(Sint16 x, Sint16 y, bool leftPressed);

	//point to the provided external objects
	Image* image = nullptr;
	RasterFont* font = nullptr;

	//positions
	Sint16 x = 0, y = 0;
	Sint16 textX = 0, textY = 0;

	//
	State state = State::NORMAL;
	std::string text;
};
