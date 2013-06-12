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
		QuitEvent();
	}
}

void OptionScreen::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}
