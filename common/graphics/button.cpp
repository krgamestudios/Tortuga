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
#include "button.hpp"

#include <stdexcept>

void Button::DrawTo(SDL_Renderer* renderer) {
	image.SetClipY(image.GetClipH() * state);
	image.DrawTo(renderer, posX, posY);
}

void Button::SetBackgroundTexture(SDL_Renderer* renderer, SDL_Texture* texture) {
	//copy the given texture
	image.Free();

	//a null texture can simply free the image
	if (!texture) {
		return;
	}

	//get the w & h, & create
	int w = 0, h = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
	image.Create(renderer, w, h);

	//copy
	SDL_SetRenderTarget(renderer, image.GetTexture());
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_SetRenderTarget(renderer, nullptr);

	//prune
	image.SetClipH(image.GetClipH() / 3);
}

void Button::SetText(SDL_Renderer* renderer, TTF_Font* font, std::string s, SDL_Color color) {
	//make the surface (from SDL_ttf)
	SDL_Surface* surf = TTF_RenderText_Solid(font, s.c_str(), color);
	if (!surf) {
		throw(std::runtime_error("Failed to create a TTF surface"));
	}

	//convert to texture
	SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (!text) {
		throw(std::runtime_error("Failed to create a TTF texture"));
	}

	//get the dimensions & rects
	int x, y, w, h;
	SDL_QueryTexture(text, nullptr, nullptr, &w, &h);
	x = (image.GetClipW() - w) / 2;
	y = (image.GetClipH() - h) / 2;
	SDL_Rect src = {0, 0, w, h};
	SDL_Rect dst;

	//draw the text to the background
	SDL_SetRenderTarget(renderer, image.GetTexture());

	for (int i = 0; i < 3; i++) {
		dst = {x, y + image.GetClipH() * i, w, h};
		SDL_RenderCopy(renderer, text, &src, &dst);
	}

	SDL_SetRenderTarget(renderer, nullptr);

	//free the texture
	SDL_DestroyTexture(text);
}

void Button::SetX(int x) {
	posX = x;
}

void Button::SetY(int y) {
	posY = y;
}

Button::State Button::MouseMotion(SDL_MouseMotionEvent const& event) {
	//if out of bounds, exit
	if (!CheckBounds(event.x, event.y)) {
		return state = State::IDLE;
	}

	//if in bounds, check button
	if (event.state & SDL_BUTTON_LMASK && state == State::PRESSED) {
		//stay pressed
//		state = State::PRESSED;
	}
	else {
		state = State::HOVER;
	}

	return state;
}

Button::State Button::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	//if out of bounds, exit
	if (!CheckBounds(event.x, event.y)) {
		return state = State::IDLE;
	}

	//if in bounds, check button
	if (event.button == SDL_BUTTON_LEFT) {
		return state = State::PRESSED;
	}

	//NOTE: if not left button down, ignore
	return State::HOVER;
}

Button::State Button::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	//if out of bounds, exit
	if (!CheckBounds(event.x, event.y)) {
		return state = State::IDLE;
	}

	//if not left button up, ignore
	if (event.button != SDL_BUTTON_LEFT) {
		return state;
	}

	//if in bounds and left button up, send release signal
	if (state == State::PRESSED) {
		state = State::HOVER;
		return State::RELEASED;
	}

	return state;
}

void Button::SetState(State s) {
	state = s;
}

Button::State Button::GetState() {
	return state;
}

bool Button::CheckBounds(int x, int y) {
	//return if true (x, y) is within bounds, otherwise return false
	return !(
		x < posX ||
		y < posY ||
		x > posX + image.GetClipW() ||
		y > posY + image.GetClipH()
		);
}