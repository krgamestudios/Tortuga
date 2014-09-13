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
#include "lobby_menu.hpp"

#include "channels.hpp"
#include "utility.hpp"

#include <stdexcept>
#include <iostream>

//-------------------------
//Public access members
//-------------------------

LobbyMenu::LobbyMenu(int* const argClientIndex, int* const argAccountIndex):
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex)
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

	//Eat incoming packets
	while(network.Receive());

	//Initial broadcast
	SendBroadcastRequest();
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

void LobbyMenu::Update() {
	//suck in and process all waiting packets
	SerialPacket* packetBuffer = reinterpret_cast<SerialPacket*>(new char[MAX_PACKET_SIZE]);
	while(network.Receive(packetBuffer)) {
		HandlePacket(packetBuffer);
	}
	delete reinterpret_cast<char*>(packetBuffer);
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
			SDL_Rect r = {
				(Sint16)listBox.x, (Sint16)listBox.y,
				(Uint16)listBox.w, (Uint16)listBox.h
			};
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

		//TODO: ping/delay?
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
		SendBroadcastRequest();
	}
	else if (join.MouseButtonUp(button) == Button::State::HOVER && selection != nullptr && selection->compatible) {
		SendJoinRequest();
	}
	else if (back.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}

	//has the user selected a server on the list?
	BoundingBox tmpBox = listBox;
	tmpBox.h *= serverInfo.size();
	if (tmpBox.CheckOverlap({button.x, button.y})) {
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

//-------------------------
//Network handlers
//-------------------------

void LobbyMenu::HandlePacket(SerialPacket* const argPacket) {
		switch(argPacket->type) {
		case SerialPacketType::BROADCAST_RESPONSE:
			HandleBroadcastResponse(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::JOIN_RESPONSE:
			HandleJoinResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::JOIN_REJECTION:
			HandleJoinRejection(static_cast<TextPacket*>(argPacket));
		break;
		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacketType encountered in LobbyMenu: " + to_string_custom(static_cast<int>(argPacket->type)) ));
		break;
	}
}

void LobbyMenu::HandleBroadcastResponse(ServerPacket* const argPacket) {
	//extract the data
	ServerInformation server;
	server.address = argPacket->srcAddress;
	server.name = argPacket->name;
	server.playerCount = argPacket->playerCount;
	server.version = argPacket->version;

	//Checking compatibility
	server.compatible = server.version == NETWORK_VERSION;

	//push
	serverInfo.push_back(server);
}

void LobbyMenu::HandleJoinResponse(ClientPacket* const argPacket) {
	clientIndex = argPacket->clientIndex;
	accountIndex = argPacket->accountIndex;
	network.Bind(argPacket->srcAddress, Channels::SERVER);
	SetNextScene(SceneList::INWORLD);
}

void LobbyMenu::HandleJoinRejection(TextPacket* const argPacket) {
	//TODO: Better output
	std::cerr << "Error: " << argPacket->text << std::endl;
}

//-------------------------
//server control
//-------------------------

void LobbyMenu::SendBroadcastRequest() {
	//broadcast to the network, or a specific server
		ClientPacket packet;
		packet.type = SerialPacketType::BROADCAST_REQUEST;
		network.SendTo(config["server.host"].c_str(), config.Int("server.port"), &packet);

		//reset the server list
		serverInfo.clear();
		selection = nullptr;
}

void LobbyMenu::SendJoinRequest() {
	//pack the packet
		ClientPacket packet;
		packet.type = SerialPacketType::JOIN_REQUEST;
		strncpy(packet.username, config["client.username"].c_str(), PACKET_STRING_SIZE);

		//join the selected server
		network.SendTo(selection->address, &packet);
		selection = nullptr;
}
