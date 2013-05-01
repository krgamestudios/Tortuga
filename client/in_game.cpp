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

	player = new Player(surfaceMgr.Get("player"), 32, 48);
}

InGame::~InGame() {
	delete player;
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
	player->Update(delta.GetDelta());
}

void InGame::Render(SDL_Surface* const screen) {
	SDL_FillRect(screen, 0, 0);
	player->DrawTo(screen);
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
			player->WalkInDirection(Direction::NORTH);
		break;
		case SDLK_s:
			player->WalkInDirection(Direction::SOUTH);
		break;
		case SDLK_a:
			player->WalkInDirection(Direction::WEST);
		break;
		case SDLK_d:
			player->WalkInDirection(Direction::EAST);
		break;
	}
}

void InGame::KeyUp(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_w:
			player->WalkInDirection(Direction::SOUTH);
		break;
		case SDLK_s:
			player->WalkInDirection(Direction::NORTH);
		break;
		case SDLK_a:
			player->WalkInDirection(Direction::EAST);
		break;
		case SDLK_d:
			player->WalkInDirection(Direction::WEST);
		break;
	}
}

//-------------------------
//Utilities
//-------------------------

void InGame::NewPlayer(int index, std::string avatarName, int x, int y) {
	//
}
