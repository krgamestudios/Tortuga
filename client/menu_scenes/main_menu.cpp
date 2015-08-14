/* Copyright: (c) Kayne Ruse 2013-2015
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

#include "config_utility.hpp"

//-------------------------
//Public access members
//-------------------------

MainMenu::MainMenu() {
	ConfigUtility& config = ConfigUtility::GetSingleton();

	//setup the utility objects
	buttonImage.Load(GetRenderer(), config["dir.interface"] + "button.png");
	font = TTF_OpenFont(config["client.font"].c_str(), 12);

	//setup the buttons
	startButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	startButton.SetText(GetRenderer(), font, "Start", {255, 255, 255, 255});
	optionsButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	optionsButton.SetText(GetRenderer(), font, "Options", {255, 255, 255, 255});
	quitButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	quitButton.SetText(GetRenderer(), font, "Quit", {255, 255, 255, 255});

	//set the button positions
	startButton.SetX(50);
	startButton.SetY(50 + 20 * 0);
	optionsButton.SetX(50);
	optionsButton.SetY(50 + 20 * 1);
	quitButton.SetX(50);
	quitButton.SetY(50 + 20 * 2);

	//text box
	textBox.PushLine(GetRenderer(), font, "Thanks for playing!", {255, 255, 255, 255});
	textBox.PushLine(GetRenderer(), font, "You can get the latest version at: ", {255, 255, 255, 255});
	textBox.PushLine(GetRenderer(), font, "krgamestudios.com", {255, 255, 255, 255});

	//debug
	//
}

MainMenu::~MainMenu() {
	TTF_CloseFont(font);
}

//-------------------------
//Frame loop
//-------------------------

void MainMenu::FrameStart() {
	//
}

void MainMenu::Update() {
	//
}

void MainMenu::FrameEnd() {
	//
}

void MainMenu::RenderFrame(SDL_Renderer* renderer) {
	startButton.DrawTo(renderer);
	optionsButton.DrawTo(renderer);
	quitButton.DrawTo(renderer);

	textBox.DrawTo(renderer, 50, 50, 12);
}

//-------------------------
//Event handlers
//-------------------------

void MainMenu::MouseMotion(SDL_MouseMotionEvent const& event) {
	startButton.MouseMotion(event);
	optionsButton.MouseMotion(event);
	quitButton.MouseMotion(event);
}

void MainMenu::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	startButton.MouseButtonDown(event);
	optionsButton.MouseButtonDown(event);
	quitButton.MouseButtonDown(event);
}

void MainMenu::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	//TODO: (2) Buttons should only register as "selected" when the left button is used
	if (startButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SetSceneSignal(SceneSignal::LOBBYMENU);
	}
	if (optionsButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SetSceneSignal(SceneSignal::OPTIONSMENU);
	}
	if (quitButton.MouseButtonUp(event) == Button::State::RELEASED) {
		QuitEvent();
	}
}

void MainMenu::KeyDown(SDL_KeyboardEvent const& event) {
	//
}

void MainMenu::KeyUp(SDL_KeyboardEvent const& event) {
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}
