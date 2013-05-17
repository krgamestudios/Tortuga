#include "lobby.hpp"

#include <exception>
#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Lobby::Lobby(ConfigUtility* cUtil, SurfaceManager* sMgr) {
#ifdef DEBUG
	cout << "entering Lobby" << endl;
#endif
	configUtil = cUtil;
	surfaceMgr = sMgr;

	//ping the network, ping the preset "phone home" servers
	//generate the server list
	//eventually

	SetNextScene(SceneList::TESTSYSTEMS);
}

Lobby::~Lobby() {
#ifdef DEBUG
	cout << "leaving Lobby" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void Lobby::FrameStart() {
	//
}

void Lobby::FrameEnd() {
	//
}

void Lobby::Update() {
	//
}

void Lobby::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void Lobby::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void Lobby::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void Lobby::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void Lobby::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void Lobby::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
