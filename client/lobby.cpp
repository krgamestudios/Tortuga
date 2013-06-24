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
#include "lobby.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Lobby::Lobby() {
#ifdef DEBUG
	cout << "entering Lobby" << endl;
#endif
	refreshButton.Setup(50, 50, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Refresh");
	joinButton.Setup(50, 100, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Join");
	backButton.Setup(50, 150, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Back");

	font.SetSurface(surfaceMgr->Get("font"));
	listBox.x = 280;
	listBox.y = 50;
	listBox.w = GetScreen()->w - listBox.x - 50;
	listBox.h = font.GetCharH();

	serverList.push_back({"foo",{0,0}});
	serverList.push_back({"bar",{0,0}});
	serverList.push_back({"foobar",{0,0}});

	flushNetworkQueue();
	beginQueueThread();
	BroadcastNetwork();
}

Lobby::~Lobby() {
#ifdef DEBUG
	cout << "leaving Lobby" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void Lobby::FrameStart() {
	//
}

void Lobby::Update(double delta) {
	while(HandlePacket(popNetworkPacket()));
}

void Lobby::FrameEnd() {
	//
}

void Lobby::Render(SDL_Surface* const screen) {
	refreshButton.DrawTo(screen);
	joinButton.DrawTo(screen);
	backButton.DrawTo(screen);

	for (int i = 0; i < serverList.size(); i++) {
		if (selectedServer == &serverList[i]) {
			//draw the highlight box
			SDL_Rect r = listBox;
			r.y += i * font.GetCharH();
			SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 255, 127, 39));
		}
		font.DrawStringTo(serverList[i].name, screen, listBox.x, listBox.y + i * font.GetCharH());
	}
}

//-------------------------
//Event handlers
//-------------------------

void Lobby::MouseMotion(SDL_MouseMotionEvent const& motion) {
	refreshButton.MouseMotion(motion);
	joinButton.MouseMotion(motion);
	backButton.MouseMotion(motion);
}

void Lobby::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	refreshButton.MouseButtonDown(button);
	joinButton.MouseButtonDown(button);
	backButton.MouseButtonDown(button);
}

void Lobby::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (refreshButton.MouseButtonUp(button) == Button::State::HOVER) {
		BroadcastNetwork();
		selectedServer = nullptr;
	}
	else if (joinButton.MouseButtonUp(button) == Button::State::HOVER) {
		if (selectedServer) {
			ConnectToServer(selectedServer);
			selectedServer = nullptr;
		}
	}
	else if (backButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
		endQueueThread();
		selectedServer = nullptr;
	}
	else if (
		//clicked within bounds TODO: make the damn collision system
		button.x > listBox.x &&
		button.y > listBox.y &&
		button.x < listBox.x + listBox.w &&
		button.y < listBox.y + (listBox.h * serverList.size())
		)
	{
		//selecting a server
		selectedServer = &serverList[(button.y - listBox.y) / listBox.h];
	}
	else {
		//lose focus on a server
		selectedServer = nullptr;
	}
}

void Lobby::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			SetNextScene(SceneList::MAINMENU);
			break;
	}
}

void Lobby::KeyUp(SDL_KeyboardEvent const& key) {
	//
}

//-------------------------
//Utilities
//-------------------------

int Lobby::HandlePacket(Packet p) {
	switch(p.meta.type) {
		case Packet::Type::NONE:
			//DO NOTHING
			return 0;
		break;
		case Packet::Type::PING:
			//quick pong
			p.meta.type = Packet::Type::PONG;
			netUtil->Send(&p.meta.address, &p, sizeof(Packet));
		break;
		case Packet::Type::PONG:
			//
		break;
//		case Packet::Type::BROADCAST_REQUEST:
//			//
//		break;
		case Packet::Type::BROADCAST_RESPONSE:
			PushServer(p);
		break;
//		case Packet::Type::JOIN_REQUEST:
//			//
//		break;
		case Packet::Type::JOIN_RESPONSE:
			BeginGame(p);
		break;
//		case Packet::Type::DISCONNECT:
//			//
//		break;
//		case Packet::Type::SYNCHRONIZE:
//			//
//		break;
//		case Packet::Type::PLAYER_NEW:
//			//
//		break;
//		case Packet::Type::PLAYER_DELETE:
//			//
//		break;
//		case Packet::Type::PLAYER_UPDATE:
//			//
//		break;
		default:
			throw(runtime_error("Failed to recognize the packet type: " + itos(int(p.meta.type))));
	}
	return 1;
}

void Lobby::BroadcastNetwork() {
	Packet p;
	p.meta.type = Packet::Type::BROADCAST_REQUEST;
	netUtil->Send("255.255.255.255", configUtil->Int("server.port"), &p, sizeof(Packet));
	serverList.clear();
}

void Lobby::PushServer(Packet& bcast) {
	ServerEntry entry;
	entry.name = bcast.serverInfo.name;
	entry.address = bcast.meta.address;
	serverList.push_back(entry);
}

void Lobby::ConnectToServer(ServerEntry* server) {
	//_attempt_ to connect to a server
	if (!server) {
		throw(runtime_error("No server received"));
	}
	Packet p;
	p.meta.type = Packet::Type::JOIN_REQUEST;
	netUtil->Send(&server->address, reinterpret_cast<void*>(&p), sizeof(Packet));
}

void Lobby::BeginGame(Packet& response) {
	//should be downloading the resources here as well
	infoMgr->SetClientIndex(response.meta.clientIndex);
	netUtil->Bind(&response.meta.address, GAME_CHANNEL);
	SetNextScene(SceneList::INWORLD);
}