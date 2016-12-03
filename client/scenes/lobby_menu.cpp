/* Copyright: (c) Kayne Ruse 2013-2016
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

#include <cstring>
#include <stdexcept>
#include <sstream>

constexpr SDL_Color WHITE = {255, 255, 255, 255};

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
	buttonImage.Load(GetRenderer(), config["dir.interface"] + "button_blue.png");
	font = TTF_OpenFont(config["client.font"].c_str(), 12);

	//check that the font loaded
	if (!font) {
		std::ostringstream msg;
		msg << "Failed to load a font file; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//setup the buttons
	searchButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	searchButton.SetText(GetRenderer(), font, WHITE, "Search");
	joinButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	joinButton.SetText(GetRenderer(), font, WHITE, "Join");
	backButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	backButton.SetText(GetRenderer(), font, WHITE, "Back");

	//set the button positions (assumed)
	searchButton.SetX(50);
	searchButton.SetY(50);
	joinButton.SetX(50);
	joinButton.SetY(70);
	backButton.SetX(50);
	backButton.SetY(90);

	//setup the text fields
	username.SetText(GetRenderer(), font, WHITE, config["client.username"]);
	password.SetText(GetRenderer(), font, WHITE, config["client.password"]);
	handle.SetText(GetRenderer(), font, WHITE, config["client.handle"]);
	avatar.SetText(GetRenderer(), font, WHITE, config["client.avatar"]);

	username.SetBounds(BoundingBox{0, 0, 300, 20});
	password.SetBounds(BoundingBox{0, 0, 300, 20});
	handle.SetBounds(BoundingBox{0, 0, 300, 20});
	avatar.SetBounds(BoundingBox{0, 0, 300, 20});

	username.SetX(50);
	username.SetY(110);
	password.SetX(50);
	password.SetY(130);
	handle.SetX(50);
	handle.SetY(150);
	avatar.SetX(50);
	avatar.SetY(170);

	//pseudo-list selection
	//TODO: move this into the UI library?
	boundingBox = {300, 50, 200, 12};

	//hacked together a highlight box
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

	username.DrawTo(renderer);
	password.DrawTo(renderer);
	handle.DrawTo(renderer);
	avatar.DrawTo(renderer);

	//TODO: (3) draw headers for the server list
	//TODO: (3) ping/delay displayed in the server list
	for (int i = 0; i < serverVector.size(); i++) {
		//draw the selected server's highlight
		if (selection == &serverVector[i]) {
			highlightImage.DrawTo(renderer, boundingBox.x, boundingBox.y + boundingBox.h * i);
		}

		//draw the server's info
		serverVector[i].nameImage.SetX(boundingBox.x);
		serverVector[i].nameImage.SetY(boundingBox.y + boundingBox.h * i);
		serverVector[i].nameImage.DrawTo(renderer);

		serverVector[i].playerCountImage.SetX(boundingBox.x+276);
		serverVector[i].playerCountImage.SetY(boundingBox.y + boundingBox.h * i);
		serverVector[i].playerCountImage.DrawTo(renderer);
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

	if (username.MouseButtonDown(event)) {
		SDL_StartTextInput();
	}
	if (password.MouseButtonDown(event)) {
		SDL_StartTextInput();
	}
	if (handle.MouseButtonDown(event)) {
		SDL_StartTextInput();
	}
	if (avatar.MouseButtonDown(event)) {
		SDL_StartTextInput();
	}
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

		case SDLK_BACKSPACE:
			//easier than mucking about with SDL_TextEditEvent
			if (username.GetFocus()) {
				username.PopChars(GetRenderer(), font, WHITE, 1);
			}
			if (password.GetFocus()) {
				password.PopChars(GetRenderer(), font, WHITE, 1);
			}
			if (handle.GetFocus()) {
				handle.PopChars(GetRenderer(), font, WHITE, 1);
			}
			if (avatar.GetFocus()) {
				avatar.PopChars(GetRenderer(), font, WHITE, 1);
			}
		break;
	}
}

void LobbyMenu::KeyUp(SDL_KeyboardEvent const& event) {
	//
}

void LobbyMenu::TextInput(SDL_TextInputEvent const& event) {
	if (username.GetFocus()) {
		username.PushText(GetRenderer(), font, WHITE, std::string(event.text));
	}
	if (password.GetFocus()) {
		password.PushText(GetRenderer(), font, WHITE, std::string(event.text));
	}
	if (handle.GetFocus()) {
		handle.PushText(GetRenderer(), font, WHITE, std::string(event.text));
	}
	if (avatar.GetFocus()) {
		avatar.PushText(GetRenderer(), font, WHITE, std::string(event.text));
	}
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
		sprintf(str, "%d", i);
		return std::string(str);
	};

	//text graphics
	serverVector.back().nameImage.SetText(GetRenderer(), font, color, newServer.name);
	serverVector.back().playerCountImage.SetText(GetRenderer(), font, color, itoa_base10(newServer.playerCount));
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
	//broadcast to the home server, and to the LAN
	ClientPacket packet;

	packet.type = SerialPacketType::BROADCAST_REQUEST;
	network.SendTo(config["server.home"].c_str(), config.Int("server.port"), &packet);
	network.SendTo(config["server.host"].c_str(), config.Int("server.port"), &packet);

	//reset the server list
	serverVector.clear();
	selection = nullptr;
}

void LobbyMenu::SendJoinRequest() {
	//BUG: 101 received in LobbyMenu on failed join
	//pack the packet
	ClientPacket packet;
	packet.type = SerialPacketType::JOIN_REQUEST;

	//join the selected server
	network.SendTo(selection->address, &packet);
	selection = nullptr;
}

void LobbyMenu::SendLoginRequest() {
	//NOTE: high cohesion
	//TODO: (9) have a separate login screen
	ClientPacket packet;
	packet.type = SerialPacketType::LOGIN_REQUEST;
	packet.clientIndex = clientIndex;
	strncpy(packet.username, username.GetText().c_str(), PACKET_STRING_SIZE+1);

	network.SendTo(Channels::SERVER, &packet);

	//TODO: remove
	config["client.username"] = username.GetText();
	config["client.password"] = password.GetText();
	config["client.handle"] = handle.GetText();
	config["client.avatar"] = avatar.GetText();
}
