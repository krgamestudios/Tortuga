#include "in_game.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

InGame::InGame() {
#ifdef DEBUG
	cout << "entering InGame" << endl;
#endif
	surfaceMgr.Load("player", "rsc/graphics/sprites/elliot2.bmp");
	surfaceMgr.Load("flower", "rsc/graphics/sprites/aniflower.bmp");

	playerCounter = currentPlayer = 0;

	playerMgr.New(playerCounter++, surfaceMgr["player"]);
	playerMgr.New(playerCounter++, surfaceMgr["player"]);
	playerMgr.New(playerCounter++, surfaceMgr["player"]);
	playerMgr.New(playerCounter++, surfaceMgr["player"]);
}

InGame::~InGame() {
	playerMgr.DeleteAll();
	surfaceMgr.FreeAll();
#ifdef DEBUG
	cout << "leaving InGame" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void InGame::FrameStart() {
	frameRate.Calculate();
}

void InGame::FrameEnd() {
	//
}

void InGame::Update() {
	delta.Calculate();
	playerMgr.UpdateAll(delta.GetDelta());
}

void InGame::Render(SDL_Surface* const screen) {
	playerMgr.DrawAllTo(screen);
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

		case SDLK_w:
			playerMgr[currentPlayer]->WalkInDirection(Direction::NORTH);
		break;
		case SDLK_s:
			playerMgr[currentPlayer]->WalkInDirection(Direction::SOUTH);
		break;
		case SDLK_a:
			playerMgr[currentPlayer]->WalkInDirection(Direction::WEST);
		break;
		case SDLK_d:
			playerMgr[currentPlayer]->WalkInDirection(Direction::EAST);
		break;

		case SDLK_1:
			SwitchToPlayer(0);
		break;
		case SDLK_2:
			SwitchToPlayer(1);
		break;
		case SDLK_3:
			SwitchToPlayer(2);
		break;
		case SDLK_4:
			SwitchToPlayer(3);
		break;
	}
}

void InGame::KeyUp(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_w:
			playerMgr[currentPlayer]->WalkInDirection(Direction::SOUTH);
		break;
		case SDLK_s:
			playerMgr[currentPlayer]->WalkInDirection(Direction::NORTH);
		break;
		case SDLK_a:
			playerMgr[currentPlayer]->WalkInDirection(Direction::EAST);
		break;
		case SDLK_d:
			playerMgr[currentPlayer]->WalkInDirection(Direction::WEST);
		break;
	}
}

//-------------------------
//Utilities
//-------------------------

void InGame::NewPlayer(int index, std::string avatarName, int x, int y) {
	Player* p = playerMgr.New(index, surfaceMgr[avatarName]);
	p->SetPosition(Vector2(x, y));
}

void InGame::SwitchToPlayer(int index) {
	//dirty hacks for smooth movement
	playerMgr[currentPlayer]->SetMotion(Vector2(0,0));
	currentPlayer = index;

	Uint8* key = SDL_GetKeyState(NULL);

	if (key[SDLK_w]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::NORTH);
	}
	if (key[SDLK_s]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::SOUTH);
	}
	if (key[SDLK_a]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::WEST);
	}
	if (key[SDLK_d]) {
		playerMgr[currentPlayer]->WalkInDirection(Direction::EAST);
	}
}