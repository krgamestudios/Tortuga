#include "test_systems.hpp"

//-------------------------
//Public access members
//-------------------------

TestSystems::TestSystems() {
	//
}

TestSystems::~TestSystems() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void TestSystems::FrameStart() {
	//
}

void TestSystems::FrameEnd() {
	//
}

void TestSystems::Update() {
	//
}

void TestSystems::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void TestSystems::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void TestSystems::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void TestSystems::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void TestSystems::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void TestSystems::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
