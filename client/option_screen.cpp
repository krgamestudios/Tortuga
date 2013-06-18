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
#include "option_screen.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

OptionScreen::OptionScreen() {
#ifdef DEBUG
	cout << "entering OptionScreen" << endl;
#endif
	backButton.Setup(50, 50, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Back");
}

OptionScreen::~OptionScreen() {
#ifdef DEBUG
	cout << "leaving OptionScreen" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void OptionScreen::Render(SDL_Surface* const screen) {
	backButton.DrawTo(screen);
}

//-------------------------
//Event handlers
//-------------------------

void OptionScreen::MouseMotion(SDL_MouseMotionEvent const& motion) {
	backButton.MouseMotion(motion);
}

void OptionScreen::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	backButton.MouseButtonDown(button);
}

void OptionScreen::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (backButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}
}

void OptionScreen::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			SetNextScene(SceneList::MAINMENU);
			break;
	}
}
