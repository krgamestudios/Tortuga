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
#include "text_field.hpp"

#include "render_text_texture.hpp"

TextField::TextField() {
	//
}

TextField::~TextField() {
	SDL_DestroyTexture(texture);
}

void TextField::DrawTo(SDL_Renderer* renderer) {
	if (!texture) {
		return;
	}
	SDL_Rect dclip = {posX, posY, 0, 0};
	SDL_QueryTexture(texture, nullptr, nullptr, &dclip.w, &dclip.h);
	SDL_RenderCopy(renderer, texture, nullptr, &dclip);
}

std::string TextField::PushText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, std::string s) {
	text += s;
	return SetText(renderer, font, color, text);
}

std::string TextField::SetText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, std::string s) {
	text = s;
	SDL_DestroyTexture(texture);
	if (text.size()) {
		texture = renderTextTexture(renderer, font, color, text);
	}
	else {
		texture = nullptr;
	}
	return text;
}

std::string TextField::PopChars(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int i) {
	if (text.size() > 0) {
		text.erase(text.size() - i);
	}
	return SetText(renderer, font, color, text);
}

std::string TextField::GetText() {
	return text;
}

bool TextField::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	BoundingBox cursorBox = {event.x, event.y, 0, 0};
	BoundingBox fieldBox = bounds;
	fieldBox.x += posX;
	fieldBox.y += posY;
	return focus = fieldBox.CheckOverlap(cursorBox);
}

BoundingBox TextField::SetBounds(BoundingBox b) {
	return bounds = b;
}

BoundingBox TextField::GetBounds() {
	return bounds;
}

bool TextField::SetFocus(bool b) {
	return focus = b;
}

bool TextField::GetFocus() {
	return focus;
}

int TextField::SetX(int i) {
	return posX = i;
}

int TextField::SetY(int i) {
	return posY = i;
}

int TextField::GetX() const {
	return posX;
}

int TextField::GetY() const {
	return posY;
}
