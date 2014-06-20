/* Copyright: (c) Kayne Ruse 2013, 2014
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
#include "shell_scene.hpp"

#include <iostream>

//-------------------------
//Public access members
//-------------------------

static double max = 0;
static double min = 0;

void setPixel(SDL_Surface* const dest, int x, int y, int colour) {
	*(static_cast<int*>(dest->pixels) + dest->w * y + x) = colour;
}

int convertToColour(SDL_PixelFormat* format, double x) {
	//track the max value
	max = x > max ? x : max;
	min = x < min ? x : min;

	if (x > 1) {
		return SDL_MapRGB(format, 255, 0, 0);
	}

	if (x < 0) {
		return SDL_MapRGB(format, 0, 255, 0);
	}

	if (x == 0) {
		return SDL_MapRGB(format, 0, 0, 255);
	}

	return SDL_MapRGB(format, 255*x, 255*x, 255*x);
}

ShellScene::ShellScene() {
	//test the generator
	int width = 256;
	int height = 256;
	image.CreateSurface(GetScreen()->w, GetScreen()->h);

	double value;
	int colour;

	std::cout << "Beggining generation" << std::endl;
	for (int i = 0; i < image.GetSurface()->w; i++) {
		for (int j = 0; j < image.GetSurface()->h; j++) {
			value = generator.ScaleOctave(i, j, width, height, 8);
			colour = convertToColour(image.GetSurface()->format, value);
			setPixel(image.GetSurface(), i, j, colour);
		}
	}
	std::cout << "Finished generation" << std::endl;
	std::cout << "Max: " << max << std::endl;
	std::cout << "Min: " << min << std::endl;
}

ShellScene::~ShellScene() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void ShellScene::FrameStart() {
	//
}

void ShellScene::Update(double delta) {
	//
}

void ShellScene::FrameEnd() {
	//
}

void ShellScene::Render(SDL_Surface* const screen) {
	image.DrawTo(screen, 0, 0);
}

//-------------------------
//Event handlers
//-------------------------

void ShellScene::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void ShellScene::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void ShellScene::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void ShellScene::KeyDown(SDL_KeyboardEvent const& key) {
	//
}

void ShellScene::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
