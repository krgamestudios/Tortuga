#include "splash.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Splash::Splash() {
#ifdef DEBUG
	cout << "entering Splash" << endl;
#endif
}

Splash::~Splash() {
#ifdef DEBUG
	cout << "leaving Splash" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void Splash::FrameStart() {
	//
}

void Splash::FrameEnd() {
	//
}

void Splash::Update() {
	//
}

void Splash::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void Splash::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void Splash::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void Splash::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void Splash::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void Splash::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
