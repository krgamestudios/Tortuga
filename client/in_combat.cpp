#include "in_combat.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

InCombat::InCombat() {
#ifdef DEBUG
	cout << "entering InCombat" << endl;
#endif
}

InCombat::~InCombat() {
#ifdef DEBUG
	cout << "leaving InCombat" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void InCombat::FrameStart() {
	//
}

void InCombat::Update(double delta) {
	//
}

void InCombat::FrameEnd() {
	//
}

void InCombat::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void InCombat::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void InCombat::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void InCombat::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void InCombat::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}

void InCombat::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
