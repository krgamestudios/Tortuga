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
#include "disconnected_screen.hpp"

#include "channels.hpp"
#include "config_utility.hpp"
#include "udp_network_utility.hpp"

#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

DisconnectedScreen::DisconnectedScreen() {
	ConfigUtility& config = ConfigUtility::GetSingleton();

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

	//full reset
	UDPNetworkUtility::GetSingleton().Unbind(Channels::SERVER);

	//auto return
	startTick = std::chrono::steady_clock::now();
}

DisconnectedScreen::~DisconnectedScreen() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void DisconnectedScreen::FrameStart() {
	//
}

void DisconnectedScreen::Update() {
	if (std::chrono::steady_clock::now() - startTick > std::chrono::duration<int>(10)) {
		SetNextScene(SceneList::MAINMENU);
	}

	//Eat incoming packets
	while(UDPNetworkUtility::GetSingleton().Receive());
}

void DisconnectedScreen::FrameEnd() {
	//
}

void DisconnectedScreen::RenderFrame(SDL_Renderer* renderer) {
	ConfigUtility& config = ConfigUtility::GetSingleton();

	backButton.DrawTo(renderer);
	font.DrawStringTo(config["client.disconnectMessage"], screen, 50, 30);
}

//-------------------------
//Event handlers
//-------------------------

void DisconnectedScreen::MouseMotion(SDL_MouseMotionEvent const& motion) {
	backButton.MouseMotion(motion);
}

void DisconnectedScreen::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	backButton.MouseButtonDown(button);
}

void DisconnectedScreen::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (backButton.MouseButtonUp(button) == Button::State::RELEASED) {
		SetSceneSignal(SceneSignal::MAINMENU);
	}
}

void DisconnectedScreen::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			SetSceneSignal(SceneSignal::MAINMENU);
		break;
	}
}

void DisconnectedScreen::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
