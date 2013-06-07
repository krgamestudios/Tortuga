#include "splash_screen.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

SplashScreen::SplashScreen() {
#ifdef DEBUG
	cout << "entering SplashScreen" << endl;
#endif
}

SplashScreen::~SplashScreen() {
#ifdef DEBUG
	cout << "leaving SplashScreen" << endl;
#endif
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
