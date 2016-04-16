/* Copyright: (c) Kayne Ruse 2013-2016
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

#include "render_text_texture.hpp"

#include <stdexcept>

TextLine::TextLine() {
	//
}

TextLine::TextLine(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, std::string str, int x, int y) {
	SetText(renderer, font, color, str);
	posX = x;
	posY = y;
}

TextLine::~TextLine() {
	ClearText();
}

void TextLine::DrawTo(SDL_Renderer* renderer) {
	SDL_Rect dclip = {posX, posY, 0, 0};
	SDL_QueryTexture(texture, nullptr, nullptr, &dclip.w, &dclip.h);
	SDL_RenderCopy(renderer, texture, nullptr, &dclip);
}

void TextLine::SetText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, std::string str) {
	//just use the above global function
	SDL_DestroyTexture(texture);
	texture = renderTextTexture(renderer, font, color, str);
	pointHeight = TTF_FontHeight(font);
}

void TextLine::ClearText() {
	SDL_DestroyTexture(texture);
	pointHeight = 0;
}

int TextLine::SetX(int i) {
	return posX = i;
}

int TextLine::SetY(int i) {
	return posY = i;
}

int TextLine::GetX() const {
	return posX;
}

int TextLine::GetY() const {
	return posY;
}

int TextLine::GetPointHeight() {
	return pointHeight;
}