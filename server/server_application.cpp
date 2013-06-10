#include "server_application.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

ServerApplication::ServerApplication() {
	//
}

ServerApplication::~ServerApplication() {
	//
}

void ServerApplication::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialide SDL_net"));
	}

	configUtil.Load("rsc/config.cfg");
	netUtil.Open(configUtil.Int("serverport"), sizeof(Packet));
	running = true;
}

void ServerApplication::Proc() {
	while(running) {
		HandleNetwork();
		UpdateWorld();
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	netUtil.Close();
	SDLNet_Quit();
}

//-------------------------
//Game loop
//-------------------------

void ServerApplication::HandleNetwork() {
	Packet p;
	while(netUtil.Receive()) {
		memcpy(&p, netUtil.GetInData(), sizeof(Packet));
		switch(p.type) {
			case PacketType::PING:
				//
			break;
			case PacketType::PONG:
				//
			break;
			case PacketType::BROADCAST_REQUEST:
				//
			break;
			case PacketType::BROADCAST_RESPONSE:
				//
			break;
			case PacketType::JOIN_REQUEST:
				//
			break;
			case PacketType::JOIN_RESPONSE:
				//
			break;
			case PacketType::DISCONNECT:
				//
			break;
			case PacketType::SYNCHRONIZE:
				//
			break;
			case PacketType::PLAYER_NEW:
				//
			break;
			case PacketType::PLAYER_DELETE:
				//
			break;
			case PacketType::PLAYER_MOVE:
				//
			break;
			default:
				throw(runtime_error("Failed to recognize the packet type"));
		}
	}
}

void ServerApplication::UpdateWorld() {
	Clock::duration delta = Clock::now() - lastTick;
	lastTick = Clock::now();
	double d = double(delta.count()) / Clock::duration::period::den;
	for (auto it : players) {
		it.second.Update(d);
	}
}

//-------------------------
//Network loop
//-------------------------

//...