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
#include "text_line.hpp"

#include <stdexcept>

SDL_Texture* renderTextTexture(SDL_Renderer* renderer, TTF_Font* font, std::string str, SDL_Color color) {
	//make the surface (from SDL_ttf)
	SDL_Surface* surface = TTF_RenderText_Solid(font, str.c_str(), color);
	if (!surface) {
		throw(std::runtime_error("Failed to create a TTF surface"));
	}

	//convert to texture
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	//cleanup
	SDL_FreeSurface(surface);

	//check
	if (!texture) {
		throw(std::runtime_error("Failed to create a TTF texture"));
	}

	//NOTE: free the texture yourself
	return texture;
}

TextLine::TextLine() {
	//
}

TextLine::~TextLine() {
	ClearText();
}

void TextLine::DrawTo(SDL_Renderer* renderer, int posX, int posY) {
	SDL_Rect dclip = {posX, posY, 0, 0};
	SDL_QueryTexture(texture, nullptr, nullptr, &dclip.w, &dclip.h);
	SDL_RenderCopy(renderer, texture, nullptr, &dclip);
}

void TextLine::SetText(SDL_Renderer* renderer, TTF_Font* font, std::string str, SDL_Color color) {
	//just use the above global function
	SDL_DestroyTexture(texture);
	texture = renderTextTexture(renderer, font, str, color);
}

void TextLine::ClearText() {
	SDL_DestroyTexture(texture);
}