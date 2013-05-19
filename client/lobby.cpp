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

		cout << "receiving" << endl;

		switch(packet.type) {
			case PacketList::PONG:
				cout << "dumping..." << endl;
				cout << packet.pong.serverName << endl;
			break;
			//...
		}
	}
}

void Lobby::Render(SDL_Surface* const screen) {
	pingButton.DrawTo(screen);
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
	//ing the network
	Packet packet;
	packet.type = PacketList::PING;
	netUtil->Send("255.255.255.255", configUtil->Integer("server.port"), reinterpret_cast<void*>(&packet), sizeof(Packet));
}
