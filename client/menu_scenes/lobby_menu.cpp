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
#include "lobby_menu.hpp"

#include "channels.hpp"

#include <stdexcept>
#include <sstream>

//-------------------------
//Public access members
//-------------------------

LobbyMenu::LobbyMenu(int* const argClientIndex, int* const argAccountIndex):
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex)
{
	//preemptive reset
	clientIndex = -1;
	accountIndex = -1;

	//setup the utility objects
	buttonImage.Load(GetRenderer(), config["dir.interface"] + "button.png");
	font = TTF_OpenFont(config["client.font"].c_str(), 12);

	//check that the font loaded
	if (!font) {
		std::ostringstream msg;
		msg << "Failed to load a font file; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//setup the buttons
	searchButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	searchButton.SetText(GetRenderer(), font, "Search", {255, 255, 255, 255});
	joinButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	joinButton.SetText(GetRenderer(), font, "Join", {255, 255, 255, 255});
	backButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	backButton.SetText(GetRenderer(), font, "Back", {255, 255, 255, 255});

	//set the button positions (assumed)
	searchButton.SetX(50);
	searchButton.SetY(50);
	joinButton.SetX(50);
	joinButton.SetY(70);
	backButton.SetX(50);
	backButton.SetY(90);

	//pseudo-list selection
	boundingBox = {300, 50, 200, 12};

	//DEBUG: hacked together a highlight box
	highlightImage.Create(GetRenderer(), 300, 12, {49, 150, 5, 255});

	//Eat incoming packets
	while(network.Receive());

	//Initial broadcast
	SendBroadcastRequest();
}

LobbyMenu::~LobbyMenu() {
	TTF_CloseFont(font);
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

void LobbyMenu::RenderFrame(SDL_Renderer* renderer) {
	//TODO: (2) I need a proper UI system for the entire client and the editor

	//UI
	searchButton.DrawTo(renderer);
	joinButton.DrawTo(renderer);
	backButton.DrawTo(renderer);

	//TODO: (3) draw headers for the server list
	//TODO: (3) ping/delay displayed in the server list
	for (int i = 0; i < serverVector.size(); i++) {
		//draw the selected server's highlight
		if (selection == &serverVector[i]) {
			highlightImage.DrawTo(renderer, boundingBox.x, boundingBox.y + boundingBox.h * i);
		}

		//draw the server's info
		serverVector[i].nameImage.DrawTo(renderer, boundingBox.x, boundingBox.y + boundingBox.h * i);
		serverVector[i].playerCountImage.DrawTo(renderer, boundingBox.x, boundingBox.y + boundingBox.h * i);
	}
}

//-------------------------
//Event handlers
//-------------------------

void LobbyMenu::MouseMotion(SDL_MouseMotionEvent const& event) {
	searchButton.MouseMotion(event);
	joinButton.MouseMotion(event);
	backButton.MouseMotion(event);
}

void LobbyMenu::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	searchButton.MouseButtonDown(event);
	joinButton.MouseButtonDown(event);
	backButton.MouseButtonDown(event);
}

void LobbyMenu::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	if (searchButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SendBroadcastRequest();
	}
	if (joinButton.MouseButtonUp(event) == Button::State::RELEASED && selection && selection->compatible) {
		SendJoinRequest();
	}
	if (backButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SetSceneSignal(SceneSignal::MAINMENU);
	}

	//has the user selected a server on the list?
	BoundingBox tmpBox = boundingBox;
	tmpBox.h *= serverVector.size(); //within the list bounds
	if (tmpBox.CheckOverlap({event.x, event.y})) {
		//NOTE: this memory trick requires a vector
		selection = &serverVector[(event.y - boundingBox.y)/boundingBox.h];
	}
	else {
		selection = nullptr;
	}
}

void LobbyMenu::MouseWheel(SDL_MouseWheelEvent const& event) {
	//
}

void LobbyMenu::KeyDown(SDL_KeyboardEvent const& event) {
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			SetSceneSignal(SceneSignal::MAINMENU);
		break;
	}
}

void LobbyMenu::KeyUp(SDL_KeyboardEvent const& event) {
	//
}

//-------------------------
//Network handlers
//-------------------------

void LobbyMenu::HandlePacket(SerialPacket* const argPacket) {
		switch(argPacket->type) {
			//responses
		case SerialPacketType::BROADCAST_RESPONSE:
			HandleBroadcastResponse(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::JOIN_RESPONSE:
			HandleJoinResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::LOGIN_RESPONSE:
			HandleLoginResponse(static_cast<ClientPacket*>(argPacket));
		break;

		//rejections
		case SerialPacketType::JOIN_REJECTION:
			HandleJoinRejection(static_cast<TextPacket*>(argPacket));
		break;
		case SerialPacketType::LOGIN_REJECTION:
			HandleLoginRejection(static_cast<TextPacket*>(argPacket));
		break;

		//handle errors
		default: {
			std::ostringstream msg;
			msg << "Unknown SerialPacketType encountered in LobbyMenu: " << static_cast<int>(argPacket->type);
			throw(std::runtime_error( msg.str() ));
		}
		break;
	}
}

void LobbyMenu::HandleBroadcastResponse(ServerPacket* const argPacket) {
	//extract the data
	ServerInfo newServer;

	newServer.address = argPacket->srcAddress;
	newServer.name = argPacket->name;
	newServer.playerCount = argPacket->playerCount;
	newServer.version = argPacket->version;
	newServer.compatible = newServer.version == NETWORK_VERSION;

	//push
	serverVector.push_back(newServer);

	//BUGFIX: since TextLine lacks the memory management of Image, I'll wait until after the line is in the vector to handle these

	//fancy colors
	SDL_Color color;
	if (newServer.compatible) {
		color = {255, 255, 255, 255};
	}
	else {
		color = {255, 0, 0, 255};
	}

	//fancy itoa
	auto itoa_base10 = [](int i) -> std::string {
		char str[20];
		printf(str, "%d", i);
		return std::string(str);
	};

	//text graphics
	serverVector.back().nameImage.SetText(GetRenderer(), font, newServer.name, color);
	serverVector.back().playerCountImage.SetText(GetRenderer(), font, itoa_base10(newServer.playerCount), color);
}

void LobbyMenu::HandleJoinResponse(ClientPacket* const argPacket) {
	//save the server's data
	clientIndex = argPacket->clientIndex;
	network.Bind(argPacket->srcAddress, Channels::SERVER);

	//request login data
	SendLoginRequest();
}

void LobbyMenu::HandleLoginResponse(ClientPacket* const argPacket) {
	if (argPacket->clientIndex != clientIndex) {
		throw(std::runtime_error("Client index invalid during login"));
	}
	accountIndex = argPacket->accountIndex;
	SetSceneSignal(SceneSignal::WORLD);
}

void LobbyMenu::HandleJoinRejection(TextPacket* const argPacket) {
	//TODO: (9) LobbyMenu::HandleJoinRejection()
}

void LobbyMenu::HandleLoginRejection(TextPacket* const argPacket) {
	//TODO: (9) LobbyMenu::HandleLoginRejection
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
		serverVector.clear();
		selection = nullptr;
}

void LobbyMenu::SendJoinRequest() {
	//pack the packet
		ClientPacket packet;
		packet.type = SerialPacketType::JOIN_REQUEST;

		//join the selected server
		network.SendTo(selection->address, &packet);
		selection = nullptr;
}

void LobbyMenu::SendLoginRequest() {
	//NOTE: high cohesion
	//TODO: have a separate login screen
	ClientPacket packet;
	packet.type = SerialPacketType::LOGIN_REQUEST;
	packet.clientIndex = clientIndex;
	strncpy(packet.username, config["client.username"].c_str(), PACKET_STRING_SIZE);

	network.SendTo(Channels::SERVER, &packet);
}
