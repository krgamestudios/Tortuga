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
#include "text_util.hpp"
#include "udp_network_utility.hpp"

#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

DisconnectedScreen::DisconnectedScreen() {
	ConfigUtility& config = ConfigUtility::GetSingleton();

	//setup the utility objects
	//TODO: (1) resource tool, to prevent reloading like this
	image.Load(GetRenderer(), config["dir.interface"] + "button.png");
	font = TTF_OpenFont(config["client.font"].c_str(), 12);

	//setup the button
	backButton.SetBackgroundTexture(GetRenderer(), image.GetTexture());
	backButton.SetText(GetRenderer(), font, "Back", COLOR_WHITE);

	//set the button positions
	backButton.SetX(50);
	backButton.SetY(50);

	//full reset
	UDPNetworkUtility::GetSingleton().Unbind(Channels::SERVER);

	//auto return
	startTick = std::chrono::steady_clock::now();
}

DisconnectedScreen::~DisconnectedScreen() {
	TTF_CloseFont(font);
}

//-------------------------
//Frame loop
//-------------------------

void DisconnectedScreen::FrameStart() {
	//
}

void DisconnectedScreen::Update() {
	if (std::chrono::steady_clock::now() - startTick > std::chrono::duration<int>(10)) {
		SetSceneSignal(SceneSignal::MAINMENU);
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
	//render output message
	SDL_Texture* tex = renderPlainText(renderer, font, config["client.disconnectMessage"], COLOR_WHITE);
	int w = 0, h = 0;
	SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
	SDL_Rect d = {50, 30, w, h};
	SDL_RenderCopy(renderer, tex, nullptr, &d);
	SDL_DestroyTexture(tex);
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
