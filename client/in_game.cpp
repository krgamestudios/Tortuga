#include "in_game.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

InGame::InGame(ConfigUtility* cUtil, SurfaceManager* sMgr, UDPNetworkUtility* nUtil, int* ID) {
#ifdef DEBUG
	cout << "entering InGame" << endl;
#endif
	configUtil = cUtil;
	surfaceMgr = sMgr;
	netUtil = nUtil;
	playerID = ID;
	cout << "playerID: " << *playerID << endl;
}

InGame::~InGame() {
	PacketData p;
	p.type = PacketList::DISCONNECT;
	p.disconnect.playerID = *playerID;
	*playerID = -1;
	netUtil->Send(0, &p, sizeof(PacketData));
	netUtil->Unbind(0);
#ifdef DEBUG
	cout << "leaving InGame" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void InGame::FrameStart() {
	//
}

void InGame::FrameEnd() {
	//
}

void InGame::Update() {
	//
}

void InGame::Render(SDL_Surface* const screen) {
	//
}

//-------------------------
//Event handlers
//-------------------------

void InGame::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void InGame::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void InGame::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void InGame::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void InGame::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
