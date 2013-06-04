#include "lobby.hpp"

#include <exception>
#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Lobby::Lobby(ConfigUtility* cUtil, SurfaceManager* sMgr, UDPNetworkUtility* nUtil, int* ID) {
#ifdef DEBUG
	cout << "entering Lobby" << endl;
#endif
	//globals
	configUtil = cUtil;
	surfaceMgr = sMgr;
	netUtil = nUtil;
	playerID = ID;

	//members
	font.SetSurface(surfaceMgr->Get("font"));

	//the buttons
	buttonMap["ping"] = new Button(50, 50 , surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Refresh");
	buttonMap["join"] = new Button(50, 100, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Join");
	buttonMap["back"] = new Button(50, 150, surfaceMgr->Get("button"), surfaceMgr->Get("font"), "Back");

	//drawing information for the server list
	listBox.x = 250;
	listBox.y = 50;
	listBox.w = GetScreen()->w - listBox.x;
	listBox.h = font.GetCharH();

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

void Lobby::Update(double delta) {
	Receive();
}

void Lobby::Receive() {
	PacketData packet;
	while(netUtil->Receive()) {
		memcpy(&packet, netUtil->GetInData(), sizeof(PacketData));
		switch(packet.type) {
//			case PacketList::NONE:
//				//
//			break;
//			case PacketList::PING:
//				//
//			break;
			case PacketList::PONG:
				PushServer(&packet);
			break;
//			case PacketList::JOINREQUEST:
//				//
//			break;
			case PacketList::JOINCONFIRM: {
				PacketData p;
				memcpy(&p, netUtil->GetInData(), sizeof(PacketData));
				*playerID = p.joinConfirm.playerID;
				netUtil->Bind(&netUtil->GetInPacket()->address, 0);
				SetNextScene(SceneList::INGAME);
			}
			break;
//			case PacketList::DISCONNECT:
//				//
//			break;
//			case PacketList::SYNCHRONIZE:
//				//
//			break;
#ifdef DEBUG
			//this might not be the end of the world; it only happens when the game ping is too low
			case PacketList::NEWPLAYER:
				cout << "WARNING: new player triggered unexpectedly" << endl;
			break;
#endif
//			case PacketList::DELETEPLAYER:
//				//
//			break;
//			case PacketList::MOVEMENT:
//				//
//			break;
		}
	}
}

void Lobby::FrameEnd() {
	//
}

void Lobby::Render(SDL_Surface* const screen) {
	for (auto it : buttonMap) {
		it.second->DrawTo(screen);
	}

	//draw the server list, highlighting the selected server
	SDL_Rect clip;
	for (int i = 0; i < serverVector.size(); i++) {
		clip = listBox;
		clip.y += i * font.GetCharH();

		//if a server has  been selected, and this is the selected server
		if (selectedServer && selectedServer == &serverVector[i]) {
			SDL_FillRect(screen, &clip, SDL_MapRGB(screen->format, 255, 127, 39));
		}
		font.DrawStringTo(serverVector[i].name, screen, clip.x, clip.y);
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
		JoinRequest(selectedServer);
	}
	if (buttonMap["back"]->MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}
	//select a server (clicked within the bounds of the server box)
	if (button.x > listBox.x && button.y > listBox.y && button.y < serverVector.size() * font.GetCharH() + listBox.y) {
		selectedServer = &serverVector[(button.y-listBox.y)/font.GetCharH()];
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
	PacketData p;
	p.type = PacketList::PING;
	netUtil->Send("255.255.255.255", configUtil->Integer("server.port"), reinterpret_cast<void*>(&p), sizeof(PacketData));
	//reset the server list
	//TODO: enable this
//	serverVector.clear();
}

void Lobby::PushServer(PacketData* packet) {
	ServerData s;
	s.name = packet->pong.metadata;
	s.address = netUtil->GetInPacket()->address;
	serverVector.push_back(s);
}

void Lobby::JoinRequest(ServerData* server) {
	if (!server) {
		//CAN receive null
		return;
	}
	PacketData p;
	p.type = PacketList::JOINREQUEST;
	snprintf(p.joinRequest.handle, PACKET_STRING_SIZE, "%s", configUtil->CString("handle"));
	snprintf(p.joinRequest.avatar, PACKET_STRING_SIZE, "%s", configUtil->CString("avatar"));
	netUtil->Send(&server->address, reinterpret_cast<void*>(&p), sizeof(PacketData));
}
