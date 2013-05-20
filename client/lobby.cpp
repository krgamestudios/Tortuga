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
	pingButton.SetSurfaces(surfaceMgr->Get("button"), surfaceMgr->Get("font"));
	pingButton.SetText("Refresh");
	pingButton.SetX(50);
	pingButton.SetY(50);

	//ping the network, ping the preset "phone home" servers
	//generate the server list
	//eventually
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
	pingButton.DrawTo(screen);
	for (int i = 0; i < serverVector.size(); i++) {
		font.DrawStringTo(serverVector[i].name, screen, 50, 16*i + 100);
	}
}

//-------------------------
//Event handlers
//-------------------------

void Lobby::MouseMotion(SDL_MouseMotionEvent const& motion) {
	pingButton.MouseMotion(motion);
}

void Lobby::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	pingButton.MouseButtonDown(button);
}

void Lobby::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (pingButton.MouseButtonUp(button) == Button::State::HOVER) {
		PingNetwork();
	}
}

void Lobby::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
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
	serverVector.clear();
}

void Lobby::PushServer(Packet* packet) {
	Server s;
	s.name = packet->pong.serverName;
	s.add = netUtil->GetInPacket()->address;
	serverVector.push_back(s);
}