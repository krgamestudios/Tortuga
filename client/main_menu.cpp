/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
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
	optionsButton.Setup(50, 50 + surfaceMgr->Get("button")->h/3, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Options");
	quitButton.Setup(50, 50 + surfaceMgr->Get("button")->h/3 * 2, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Quit");
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
