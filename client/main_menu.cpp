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
	configUtil = GetSingletonPtr<ConfigUtility>();
	surfaceMgr = GetSingletonPtr<SurfaceManager>();

	surfaceMgr->Load("button", configUtil->String("interface") + "/button.bmp");

	buttonMap["start"] = new Button(50, 50, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "start");
	buttonMap["options"] = new Button(50, 100, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "options");
	buttonMap["quit"] = new Button(50, 150, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "quit");
}

MainMenu::~MainMenu() {
	for (auto it : buttonMap) {
		delete it.second;
	}
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

void MainMenu::FrameEnd() {
	//
}

void MainMenu::Update() {
	//
}

void MainMenu::Render(SDL_Surface* const screen) {
	for (auto it : buttonMap) {
		it.second->DrawTo(screen);
	}
}

//-------------------------
//Event handlers
//-------------------------

void MainMenu::MouseMotion(SDL_MouseMotionEvent const& motion) {
	for (auto it : buttonMap) {
		it.second->MouseMotion(motion);
	}
}

void MainMenu::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	for (auto it : buttonMap) {
		it.second->MouseButtonDown(button);
	}
}

void MainMenu::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (buttonMap["start"]->MouseButtonUp(button) == Button::State::HOVER) {
		//TODO
		SetNextScene(SceneList::INGAME);
		cout << "start" << endl;
	}
	if (buttonMap["options"]->MouseButtonUp(button) == Button::State::HOVER) {
		//TODO
		cout << "options" << endl;
	}
	if (buttonMap["quit"]->MouseButtonUp(button) == Button::State::HOVER) {
		//TODO
		SetNextScene(SceneList::QUIT);
		cout << "quit" << endl;
	}
}

void MainMenu::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void MainMenu::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
