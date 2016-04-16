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
#include "text_box.hpp"

#include <stdexcept>

TextBox::TextBox() {
	//
}

TextBox::~TextBox() {
	//
}

void TextBox::DrawTo(SDL_Renderer* renderer) {
	int renderY = posY;
	for (std::list<TextLine>::iterator it = lineList.begin(); it != lineList.end(); it++) {
		it->SetX(posX);
		it->SetY(renderY);
		it->DrawTo(renderer);
		renderY += it->GetPointHeight();
	}
}

void TextBox::PushLine(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, std::string str) {
	lineList.emplace_back(renderer, font, color, str, 0, 0);
}

void TextBox::PopLine(int num) {
	//prevent underflow
	num < lineList.size() ? num : lineList.size();

	for (int i = 0; i < num; ++i) {
		lineList.pop_front();
	}
}

void TextBox::ClearLines() {
	lineList.clear();
}

int TextBox::SetX(int i) {
	return posX = i;
}

int TextBox::SetY(int i) {
	return posY = i;
}

int TextBox::GetX() const {
	return posX;
}

int TextBox::GetY() const {
	return posY;
}
