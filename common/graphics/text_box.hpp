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

#include "text_line.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <string>
#include <list>

/* DOCS: TextBox is a templated class to allow additional data, read more
 * As long as the class given as an argument meets these requirements, this
 * should work:
 * 
 *   Constructor(SDL_Renderer*, TTF_Font*, std::string, SDL_Color)
 *   DrawTo(SDL_Renderer*, int posX, int posY)
 * 
 * The class should also clean up after itself.
*/

//template class definition
template<typename Line = TextLine>
class TextBox {
public:
	TextBox() = default;
	~TextBox() = default;

	void DrawTo(SDL_Renderer*, int posX, int posY, int pointSize);

	void PushLine(SDL_Renderer*, TTF_Font*, std::string, SDL_Color color);
	void PopLine(int num = 1);
	void ClearLines();

private:
	std::list<Line> lineList;
};

//method definitions
template<typename Line>
void TextBox<Line>::DrawTo(SDL_Renderer* renderer, int posX, int posY, int pointSize) {
	//NOTE: pointSize should match the font's size, but may be negative to give a dialog effect
	for (typename std::list<Line>::iterator it = lineList.begin(); it != lineList.end(); it++) {
		it->DrawTo(renderer, posX, posY);
		posY += pointSize;
	}
}

template<typename Line>
void TextBox<Line>::PushLine(SDL_Renderer* renderer, TTF_Font* font, std::string str, SDL_Color color) {
	lineList.emplace_front(renderer, font, str, color);
}

template<typename Line>
void TextBox<Line>::PopLine(int num) {
	//prevent underflow
	num < lineList.size() ? num : lineList.size();

	for (int i = 0; i < num; ++i) {
		lineList.pop_back();
	}
}

template<typename Line>
void TextBox<Line>::ClearLines() {
	lineList.clear();
}
