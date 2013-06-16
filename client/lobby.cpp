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

	queueThread = SDL_CreateThread(networkQueue, nullptr);

	if (!queueThread) {
		throw(runtime_error("Failed to create the network thread"));
	}

	BroadcastNetwork();
}

Lobby::~Lobby() {
	SDL_KillThread(queueThread);
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
	try {
		//process all packets on the network queue
		while(HandlePacket(popNetworkPacket()));
	}
	catch(exception& e) {
		cerr << "Network Error: " << e.what() << endl;
	}
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
	}
	else if (joinButton.MouseButtonUp(button) == Button::State::HOVER) {
		//TODO: join a server
	}
	else if (backButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
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
	switch(p.type) {
		case PacketType::NONE:
			//DO NOTHING
			return 0;
		break;
		case PacketType::PING:
			//quick pong
			p.type = PacketType::PONG;
			netUtil->Send(&netUtil->GetInPacket()->address, &p, sizeof(Packet));
		break;
		case PacketType::PONG:
			//
		break;
//		case PacketType::BROADCAST_REQUEST:
//			//
//		break;
		case PacketType::BROADCAST_RESPONSE:
			PushServer(p.broadcastResponse);
		break;
//		case PacketType::JOIN_REQUEST:
//			//
//		break;
//		case PacketType::JOIN_RESPONSE:
//			//
//		break;
//		case PacketType::DISCONNECT:
//			//
//		break;
//		case PacketType::SYNCHRONIZE:
//			//
//		break;
//		case PacketType::PLAYER_NEW:
//			//
//		break;
//		case PacketType::PLAYER_DELETE:
//			//
//		break;
//		case PacketType::PLAYER_MOVE:
//			//
//		break;
		default:
			throw(runtime_error("Failed to recognize the packet type"));
	}
	return 1;
}

void Lobby::BroadcastNetwork() {
	Packet p;
	p.type = PacketType::BROADCAST_REQUEST;
	netUtil->Send("255.255.255.255", configUtil->Int("server.port"), &p, sizeof(Packet));
	serverList.clear();
}

void Lobby::PushServer(BroadcastResponse& bcast) {
	ServerEntry entry;
	entry.name = bcast.name;
	entry.address = netUtil->GetInPacket()->address;
	serverList.push_back(entry);
}