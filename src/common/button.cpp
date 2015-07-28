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
	image.DrawTo(renderer, 0, 0);
}

void Button::SetBackgroundTexture(SDL_Renderer* renderer, SDL_Texture* texture) {
	//copy the given texture
	image.Free();

	//a blank texture can simply free the image
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
	int w, h;
	SDL_QueryTexture(text, nullptr, nullptr, &w, &h);

	//draw the text to the background
	SDL_Rect src = {0, 0, w, h};
	SDL_Rect dst = {
		(image.GetClipW() - w) / 2,
		(image.GetClipH() - h) / 2,
		w, h};
	SDL_SetRenderTarget(renderer, image.GetTexture());
	SDL_RenderCopy(renderer, text, &src, &dst);
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

void Button::MouseMotion(SDL_MouseMotionEvent const&) {
	//TODO: empty
}

void Button::MouseButtonDown(SDL_MouseButtonEvent const&) {
	//TODO: empty
}

void Button::MouseButtonUp(SDL_MouseButtonEvent const&) {
	//TODO: empty
}

void Button::SetOnPress(fptr arg) {
	onPress = arg;
}

void Button::SetOnRelease(fptr arg) {
	onRelease = arg;
}
