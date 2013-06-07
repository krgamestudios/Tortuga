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
}

OptionScreen::~OptionScreen() {
#ifdef DEBUG
	cout << "leaving OptionScreen" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void OptionScreen::FrameStart() {
	//
}

void OptionScreen::Update(double delta) {
	//
}

void OptionScreen::FrameEnd() {
	//
}

void OptionScreen::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void OptionScreen::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void OptionScreen::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void OptionScreen::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void OptionScreen::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}

void OptionScreen::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
