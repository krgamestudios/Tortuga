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

#include "channels.hpp"
#include "utility.hpp"

#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

LobbyMenu::LobbyMenu(ConfigUtility* const argConfig, UDPNetworkUtility* const argNetwork, int* const argClientIndex, int* const argAccountIndex, int* const argCharacterIndex):
	config(*argConfig),
	network(*argNetwork),
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex),
	characterIndex(*argCharacterIndex)
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

	//set the server list's position
	listBox = {300, 50, 200, font.GetCharH()};
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
	//suck in all waiting packets
	SerialPacket packet;
	while(network.Receive()) {
		deserialize(&packet, network.GetInData());
		packet.meta.srcAddress = network.GetInPacket()->address;
		HandlePacket(packet);
	}
}

void LobbyMenu::FrameEnd() {
	//
}

void LobbyMenu::Render(SDL_Surface* const screen) {
	//TODO: I need a proper UI system for the entire client and the editor
	//UI
	search.DrawTo(screen);
	join.DrawTo(screen);
	back.DrawTo(screen);

	//TODO: draw headers for the server list
	for (int i = 0; i < serverInfo.size(); i++) {
		//draw the selected server's highlight
		if (selection == &serverInfo[i]) {
			SDL_Rect r = listBox;
			r.y += i * listBox.h;
			SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 255, 127, 39));
		}

		//draw the server name
		font.DrawStringTo(serverInfo[i].name, screen, listBox.x, listBox.y + i*listBox.h);

		//draw the player count
		font.DrawStringTo(to_string_custom(serverInfo[i].playerCount), screen, listBox.x + listBox.w, listBox.y + i*listBox.h);

		//compatible?
		if (!serverInfo[i].compatible) {
			font.DrawStringTo("?", screen, listBox.x - font.GetCharW(), listBox.y + i*listBox.h);
		}

		//ping?
	}
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
		//the vars
		SerialPacket packet;
		char buffer[PACKET_BUFFER_SIZE];

		//broadcast to the network, or a specific server
		packet.meta.type = SerialPacket::Type::BROADCAST_REQUEST;
		serialize(&packet, buffer);
		network.Send(config["server.host"].c_str(), config.Int("server.port"), buffer, PACKET_BUFFER_SIZE);

		//reset the server list
		serverInfo.clear();
		selection = nullptr;
	}

	else if (join.MouseButtonUp(button) == Button::State::HOVER && selection != nullptr && selection->compatible) {
		//the vars
		SerialPacket packet;
		char buffer[PACKET_BUFFER_SIZE];

		//pack the packet
		packet.meta.type = SerialPacket::Type::JOIN_REQUEST;
		strncpy(packet.clientInfo.username, config["client.username"].c_str(), PACKET_STRING_SIZE);
		strncpy(packet.clientInfo.handle, config["client.handle"].c_str(), PACKET_STRING_SIZE);
		strncpy(packet.clientInfo.avatar, config["client.avatar"].c_str(), PACKET_STRING_SIZE);

		//join the selected server
		serialize(&packet, buffer);
		network.Send(&selection->address, buffer, PACKET_BUFFER_SIZE);
		selection = nullptr;
	}

	else if (back.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}

	else if (
		//has the user selected a server on the list?
		button.x > listBox.x &&
		button.x < listBox.x + listBox.w &&
		button.y > listBox.y &&
		button.y < listBox.y + listBox.h * serverInfo.size()
		) {
		selection = &serverInfo[(button.y - listBox.y)/listBox.h];
	}
	else {
		selection = nullptr;
	}
}

void LobbyMenu::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			SetNextScene(SceneList::MAINMENU);
		break;
	}
}

void LobbyMenu::KeyUp(SDL_KeyboardEvent const& key) {
	//
}

void LobbyMenu::HandlePacket(SerialPacket packet) {
		switch(packet.meta.type) {
		case SerialPacket::Type::BROADCAST_RESPONSE: {
			//extract the data
			ServerInformation server;
			server.address = packet.meta.srcAddress;
			server.networkVersion = packet.serverInfo.networkVersion;
			server.name = packet.serverInfo.name;
			server.playerCount = packet.serverInfo.playerCount;

			//NOTE: Check compatibility here
			server.compatible = server.networkVersion == NETWORK_VERSION;

			//push
			serverInfo.push_back(server);
		}
		break;
		case SerialPacket::Type::JOIN_RESPONSE:
			clientIndex = packet.clientInfo.clientIndex;
			accountIndex = packet.clientInfo.accountIndex;
			characterIndex = packet.clientInfo.characterIndex;
			network.Bind(&packet.meta.srcAddress, Channels::SERVER);
			SetNextScene(SceneList::INWORLD);
		break;

		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacket::Type encountered in LobbyMenu: " + to_string_custom(int(packet.meta.type))));
		break;
	}
}