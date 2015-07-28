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

#include "image.hpp"

#include "SDL2/SDL_ttf.h"

#include <string>

class Button {
public:
	//types
	typedef int (*fptr)();

	//methods
	Button() = default;
	~Button() = default;

	void DrawTo(SDL_Renderer*);

	//setup
	void SetBackgroundTexture(SDL_Renderer*, SDL_Texture*);
	void SetText(SDL_Renderer*, TTF_Font*, std::string, SDL_Color);
	void SetX(int x);
	void SetY(int y);

	//capture input
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);

	//responses
	void SetOnPress(fptr);
	void SetOnRelease(fptr);

protected:
	Image image;
	int posX = 0, posY = 0;
	fptr onPress = nullptr;
	fptr onRelease = nullptr;
};
