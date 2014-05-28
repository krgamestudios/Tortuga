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
#include "options_menu.hpp"

//-------------------------
//Public access members
//-------------------------

OptionsMenu::OptionsMenu(ConfigUtility* const argConfig):
	config(*argConfig)
{
	//setup the utility objects
	image.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	image.SetClipH(image.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	backButton.SetImage(&image);
	backButton.SetFont(&font);

	//set the button positions
	backButton.SetX(50);
	backButton.SetY(50 + image.GetClipH() * 0);

	//set the button texts
	backButton.SetText("Back");
}

OptionsMenu::~OptionsMenu() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void OptionsMenu::FrameStart() {
	//
}

void OptionsMenu::Update(double delta) {
	//
}

void OptionsMenu::FrameEnd() {
	//
}

void OptionsMenu::Render(SDL_Surface* const screen) {
	backButton.DrawTo(screen);

	font.DrawStringTo("Oh, were you looking for the options screen?", screen, 50, 30);
}

//-------------------------
//Event handlers
//-------------------------

void OptionsMenu::MouseMotion(SDL_MouseMotionEvent const& motion) {
	backButton.MouseMotion(motion);
}

void OptionsMenu::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	backButton.MouseButtonDown(button);
}

void OptionsMenu::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (backButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}
}

void OptionsMenu::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			SetNextScene(SceneList::MAINMENU);
		break;
	}
}

void OptionsMenu::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
