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
	if (!image || !font) {
		throw(std::runtime_error("Surface not set for Button"));
	}
	image->SetClipY(state * image->GetClipH());
	image->DrawTo(dest, x, y);
	font->DrawStringTo(text, dest, textX + x, textY + y);
}

std::string Button::SetText(std::string t) {
	if (!image || !font) {
		throw(std::runtime_error("Surface not set for Button"));
	}
	//one line, cache the position
	text = t;
	textX = (image->GetClipW() / 2) - (font->GetCharW() * text.size() / 2);
	textY = (image->GetClipH() / 2) - (font->GetCharH() / 2);
	return text;
}

Button::State Button::CalcState(Sint16 i, Sint16 j, bool leftPressed) {
	if (!image || !font) {
		throw(std::runtime_error("Surface not set for Button"));
	}
	//if out of bounds
	if (i < x || i >= (x + image->GetClipW()) ||
		j < y || j >= (y + image->GetClipH())
		) {
		return state = State::NORMAL;
	}

	if (leftPressed) {
		return state = State::PRESSED;
	}
	else {
		return state = State::HOVER;
	}
}
