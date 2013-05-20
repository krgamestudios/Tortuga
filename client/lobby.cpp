#include "lobby.hpp"

#include <exception>
#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Lobby::Lobby(ConfigUtility* cUtil, SurfaceManager* sMgr, UDPNetworkUtility* nUtil) {
#ifdef DEBUG
	cout << "entering Lobby" << endl;
#endif
	//globals
	configUtil = cUtil;
	surfaceMgr = sMgr;
	netUtil = nUtil;

	//members
	font.SetSurface(surfaceMgr->Get("font"));

	buttonMap["ping"] = new Button(50, 50 , surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Refresh");
	buttonMap["join"] = new Button(50, 100, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Join");
	buttonMap["back"] = new Button(50, 150, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Back");

	//ping the network
	PingNetwork();
}

Lobby::~Lobby() {
	for (auto it : buttonMap) {
		delete it.second;
	}
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

void Lobby::FrameEnd() {
	//
}

void Lobby::Update() {
	Receive();
}

void Lobby::Receive() {
	//dump to the console
	Packet packet;
	while(netUtil->Receive()) {
		memcpy(&packet, netUtil->GetInData(), sizeof(Packet));
		switch(packet.type) {
			case PacketList::PONG:
				PushServer(&packet);
			break;
			//...
		}
	}
}

void Lobby::Render(SDL_Surface* const screen) {
	for (auto it : buttonMap) {
		it.second->DrawTo(screen);
	}
	//draw the server list, highlighting the selected server
	for (int i = 0; i < serverVector.size(); i++) {
		if (selectedServer && selectedServer == &serverVector[i]) {
			SDL_Rect clip = {250, Sint16(16*i + 50), Uint16(screen->w - 250), 16};
			SDL_FillRect(screen, &clip, SDL_MapRGB(screen->format, 255, 127, 39));
		}
		font.DrawStringTo(serverVector[i].name, screen, 250, 16*i + 50);
	}
}

//-------------------------
//Event handlers
//-------------------------

void Lobby::MouseMotion(SDL_MouseMotionEvent const& motion) {
	for (auto it : buttonMap) {
		it.second->MouseMotion(motion);
	}
}

void Lobby::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	for (auto it : buttonMap) {
		it.second->MouseButtonDown(button);
	}
}

void Lobby::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (buttonMap["ping"]->MouseButtonUp(button) == Button::State::HOVER) {
		PingNetwork();
	}
	if (buttonMap["join"]->MouseButtonUp(button) == Button::State::HOVER) {
		//join...
		if (selectedServer) {
			cout << "joining server: " << selectedServer->name << endl;
		}
	}
	if (buttonMap["back"]->MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}
	//select a server
	if (button.x >= 250 && button.y >= 50 && button.y < serverVector.size() * 16 + 50) {
		selectedServer = &serverVector[(button.y-50)/16];
	}
	else {
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

void Lobby::PingNetwork() {
	//ping the network
	Packet packet;
	packet.type = PacketList::PING;
	netUtil->Send("255.255.255.255", configUtil->Integer("server.port"), reinterpret_cast<void*>(&packet), sizeof(Packet));
	//reset the server list
//	serverVector.clear();
}

void Lobby::PushServer(Packet* packet) {
	Server s;
	s.name = packet->pong.serverName;
	s.add = netUtil->GetInPacket()->address;
	serverVector.push_back(s);
}