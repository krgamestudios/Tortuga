#include "main_menu.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

MainMenu::MainMenu() {
#ifdef DEBUG
	cout << "entering MainMenu" << endl;
#endif
	startButton.Setup(50, 50, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Start");
	optionsButton.Setup(50, 100, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Options");
	quitButton.Setup(50, 150, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Quit");
}

MainMenu::~MainMenu() {
#ifdef DEBUG
	cout << "leaving MainMenu" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void MainMenu::FrameStart() {
	//
}

void MainMenu::Update(double delta) {
	//
}

void MainMenu::FrameEnd() {
	//
}

void MainMenu::Render(SDL_Surface* const screen) {
	startButton.DrawTo(screen);
	optionsButton.DrawTo(screen);
	quitButton.DrawTo(screen);
}

//-------------------------
//Event handlers
//-------------------------

void MainMenu::MouseMotion(SDL_MouseMotionEvent const& motion) {
	startButton.MouseMotion(motion);
	optionsButton.MouseMotion(motion);
	quitButton.MouseMotion(motion);
}

void MainMenu::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	startButton.MouseButtonDown(button);
	optionsButton.MouseButtonDown(button);
	quitButton.MouseButtonDown(button);
}

void MainMenu::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (startButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::LOBBY);
	}
	if (optionsButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::OPTIONSCREEN);
	}
	if (quitButton.MouseButtonUp(button) == Button::State::HOVER) {
		QuitEvent();
	}
}

void MainMenu::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}
