#include "main_menu.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

MainMenu::MainMenu(ConfigUtility* cUtil, SurfaceManager* sMgr) {
#ifdef DEBUG
	cout << "entering MainMenu" << endl;
#endif
	configUtil = cUtil;
	surfaceMgr = sMgr;

	buttonMap["start"] = new Button(50, 50, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Start");
	buttonMap["options"] = new Button(50, 100, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Options");
	buttonMap["quit"] = new Button(50, 150, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Quit");

	buttonMap["testsystems"] = new Button(50, 250, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "TestSystems");
}

MainMenu::~MainMenu() {
	for (auto it : buttonMap) {
		delete it.second;
	}
	buttonMap.clear();
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
		SetNextScene(SceneList::LOBBY);
	}
	if (buttonMap["options"]->MouseButtonUp(button) == Button::State::HOVER) {
		//TODO
	}
	if (buttonMap["quit"]->MouseButtonUp(button) == Button::State::HOVER) {
		QuitEvent();
	}
	if (buttonMap["testsystems"]->MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::TESTSYSTEMS);
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
