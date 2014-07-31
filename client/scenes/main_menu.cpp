/* Copyright: (c) Kayne Ruse 2013, 2014
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

//-------------------------
//Public access members
//-------------------------

MainMenu::MainMenu(ConfigUtility* const argConfig):
	config(*argConfig)
{
	//setup the utility objects
	image.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	image.SetClipH(image.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	startButton.SetImage(&image);
	startButton.SetFont(&font);
	optionsButton.SetImage(&image);
	optionsButton.SetFont(&font);
	quitButton.SetImage(&image);
	quitButton.SetFont(&font);

	//set the button positions
	startButton.SetX(50);
	startButton.SetY(50 + image.GetClipH() * 0);
	optionsButton.SetX(50);
	optionsButton.SetY(50 + image.GetClipH() * 1);
	quitButton.SetX(50);
	quitButton.SetY(50 + image.GetClipH() * 2);

	//set the button texts
	startButton.SetText("Start");
	optionsButton.SetText("Options");
	quitButton.SetText("Quit");

	//debug
	//
}

MainMenu::~MainMenu() {
	//
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
	//TODO: Buttons should only register as "selected" when the left button is used
	if (startButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::LOBBYMENU);
	}
	if (optionsButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::OPTIONSMENU);
	}
	if (quitButton.MouseButtonUp(button) == Button::State::HOVER) {
		QuitEvent();
	}
}

void MainMenu::KeyDown(SDL_KeyboardEvent const& key) {
	//
}

void MainMenu::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
