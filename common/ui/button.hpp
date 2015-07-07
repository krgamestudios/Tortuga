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

#include "bounding_box.hpp"
#include "image.hpp"

#include "SDL2/SDL_ttf.h"

#include <string>

class Button {
public:
	//states available
	enum class State {
		NORMAL, HOVER, PRESSED
	};

	//methods
	Button() = default;
	~Button() = default;

	void RenderText(std::string s);
	bool CaptureInput(int x, int y, bool pressed);
	void DrawTo(SDL_Renderer*, int camX, int camY);

	//accessors & mutators
	int SetX(int x) { return posX = x; }
	int SetY(int y) { return posY = y; }
	int GetX() { return posX; }
	int GetY() { return posY; }
	Image* GetImage() { return &image; }
	BoundingBox* GetBoundingBox() { return &boundingBox; }

protected:
	Image image;
	BoundingBox boundingBox;
	int posX = 0, posY = 0;
};
