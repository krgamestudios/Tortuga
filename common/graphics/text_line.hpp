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
#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <string>

class TextLine {
public:
	TextLine();
	TextLine(SDL_Renderer*, TTF_Font*, SDL_Color, std::string, int x, int y);
	virtual ~TextLine();

	void DrawTo(SDL_Renderer*);

	void SetText(SDL_Renderer*, TTF_Font*, SDL_Color, std::string);
	void ClearText();

	//accessors & mutators
	int SetX(int i);
	int SetY(int i);
	int GetX() const;
	int GetY() const;

	//utility
	int GetPointHeight();

protected:
	SDL_Texture* texture = nullptr;
	int posX = 0, posY = 0;
	int pointHeight = 0; //internal use for TextBox
};