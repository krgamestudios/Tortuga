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
#include "options_menu.hpp"

#include "config_utility.hpp"

#include <sstream>
#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

OptionsMenu::OptionsMenu() {
	ConfigUtility& config = ConfigUtility::GetSingleton();

	//setup the utility objects
	buttonImage.Load(GetRenderer(), config["dir.interface"] + "button.png");
	font = TTF_OpenFont(config["client.font"].c_str(), 12);

	//check that the font loaded
	if (!font) {
		std::ostringstream msg;
		msg << "Failed to load a font file; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//setup the button
	backButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	backButton.SetText(GetRenderer(), font, "Back", {255, 255, 255, 255});

	//set the button positions
	backButton.SetX(50);
	backButton.SetY(50);

	//text line
	textLine.SetText(GetRenderer(), font, "This code is fucking hard to refactor.", {255, 255, 255, 255});
}

OptionsMenu::~OptionsMenu() {
	TTF_CloseFont(font);
}

//-------------------------
//Frame loop
//-------------------------

void OptionsMenu::FrameStart() {
	//
}

void OptionsMenu::Update() {
	//
}

void OptionsMenu::FrameEnd() {
	//
}

void OptionsMenu::RenderFrame(SDL_Renderer* renderer) {
	backButton.DrawTo(renderer);
	textLine.DrawTo(renderer, 50, 30);
}

//-------------------------
//Event handlers
//-------------------------

void OptionsMenu::MouseMotion(SDL_MouseMotionEvent const& event) {
	backButton.MouseMotion(event);
}

void OptionsMenu::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	backButton.MouseButtonDown(event);
}

void OptionsMenu::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	if (backButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SetSceneSignal(SceneSignal::MAINMENU);
	}
}

void OptionsMenu::MouseWheel(SDL_MouseWheelEvent const& event) {
	//
}

void OptionsMenu::KeyDown(SDL_KeyboardEvent const& event) {
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			SetSceneSignal(SceneSignal::MAINMENU);
		break;
	}
}

void OptionsMenu::KeyUp(SDL_KeyboardEvent const& event) {
	//
}
