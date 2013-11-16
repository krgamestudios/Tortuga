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
#include "lobby_menu.hpp"

//-------------------------
//Public access members
//-------------------------

LobbyMenu::LobbyMenu(ConfigUtility* const arg1, UDPNetworkUtility* const arg2):
	config(*arg1),
	network(*arg2)
{
	//setup the utility objects
	image.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	image.SetClipH(image.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	search.SetImage(&image);
	search.SetFont(&font);
	join.SetImage(&image);
	join.SetFont(&font);
	back.SetImage(&image);
	back.SetFont(&font);

	//set the button positions
	search.SetX(50);
	search.SetY(50 + image.GetClipH() * 0);
	join.SetX(50);
	join.SetY(50 + image.GetClipH() * 1);
	back.SetX(50);
	back.SetY(50 + image.GetClipH() * 2);

	//set the button texts
	search.SetText("Search");
	join.SetText("Join");
	back.SetText("Back");
}

LobbyMenu::~LobbyMenu() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void LobbyMenu::FrameStart() {
	//
}

void LobbyMenu::Update(double delta) {
	//
}

void LobbyMenu::FrameEnd() {
	//
}

void LobbyMenu::Render(SDL_Surface* const screen) {
	search.DrawTo(screen);
	join.DrawTo(screen);
	back.DrawTo(screen);
}

//-------------------------
//Event handlers
//-------------------------

void LobbyMenu::MouseMotion(SDL_MouseMotionEvent const& motion) {
	search.MouseMotion(motion);
	join.MouseMotion(motion);
	back.MouseMotion(motion);
}

void LobbyMenu::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	search.MouseButtonDown(button);
	join.MouseButtonDown(button);
	back.MouseButtonDown(button);
}

void LobbyMenu::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (search.MouseButtonUp(button) == Button::State::HOVER) {
		//ping the LAN
		NetworkPacket packet;
		packet.meta.type = NetworkPacket::Type::BROADCAST_REQUEST;
		network.Send(config["server.host"].c_str(), config.Int("server.port"), reinterpret_cast<void*>(&packet), sizeof(NetworkPacket));
	}
	if (join.MouseButtonUp(button) == Button::State::HOVER) {
		//join the selected server
	}
	if (back.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}
}

void LobbyMenu::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}

void LobbyMenu::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
