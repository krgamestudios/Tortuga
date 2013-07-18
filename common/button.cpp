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
#include "button.hpp"

#include <stdexcept>

void Button::Setup(Sint16 i, Sint16 j, SDL_Surface* bg, SDL_Surface* fg, std::string t) {
	x = i;
	y = j;
	SetSurfaces(bg, fg);
	SetText(t);
}

Button::State Button::MouseMotion(SDL_MouseMotionEvent const& motion) {
	return CalcState(motion.x, motion.y, motion.state & SDL_BUTTON_LMASK);
}

Button::State Button::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	if (button.button == SDL_BUTTON_LEFT) {
		return CalcState(button.x, button.y, true);
	}
	return state;
}

Button::State Button::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (button.button == SDL_BUTTON_LEFT) {
		return CalcState(button.x, button.y, false);
	}
	return state;
}

void Button::DrawTo(SDL_Surface* const dest) {
	image.DrawTo(dest, x, y);
	font.DrawStringTo(text, dest, textX + x, textY + y);
}

void Button::SetSurfaces(SDL_Surface* bg, SDL_Surface* fg) {
	//graphical stuff
	image.SetSurface(bg);
	image.SetClipH(image.GetClipH() / 3); //3 phases, vertical storage
	font.SetSurface(fg);

	//reset textX & textY
	SetText(text);
}

std::string Button::SetText(std::string t) {
	//one line, cache the position
	text = t;
	textX = (image.GetClipW() / 2) - (font.GetCharW() * text.size() / 2);
	textY = (image.GetClipH() / 2) - (font.GetCharH() / 2);
	return text;
}

Button::State Button::CalcState(Sint16 i, Sint16 j, bool leftPressed) {
	if (i < x || i > (x + image.GetClipW()) ||
		j < y || j > (y + image.GetClipH())
		) {
		image.SetClipY(0);
		return state = State::NORMAL;
	}
	if (leftPressed) {
		image.SetClipY(image.GetClipH()*2);
		return state = State::PRESSED;
	}
	else {
		image.SetClipY(image.GetClipH());
		return state = State::HOVER;
	}
}
