/* Copyright: (c) Kayne Ruse 2013
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
#include "splash_screen.hpp"

//-------------------------
//Public access members
//-------------------------

SplashScreen::SplashScreen() {
	//
}

SplashScreen::~SplashScreen() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void SplashScreen::FrameStart() {
	//
}

void SplashScreen::Update(double delta) {
	//
}

void SplashScreen::FrameEnd() {
	//
}

void SplashScreen::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void SplashScreen::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void SplashScreen::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void SplashScreen::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void SplashScreen::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}

void SplashScreen::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
