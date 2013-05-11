#include "combat.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Combat::Combat() {
#ifdef DEBUG
	cout << "entering Combat" << endl;
#endif
}

Combat::~Combat() {
#ifdef DEBUG
	cout << "leaving Combat" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void Combat::FrameStart() {
	//
}

void Combat::FrameEnd() {
	//
}

void Combat::Update() {
	//
}

void Combat::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void Combat::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void Combat::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void Combat::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void Combat::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void Combat::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
