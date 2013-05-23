#include "server_application.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

void ServerApplication::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialize SDL_net"));
	}
	configUtil.Load("config.cfg");
	netUtil.Open(configUtil.Integer("server.port"), sizeof(PacketData));
	running = true;
}

void ServerApplication::Proc() {
	while(running) {
		HandleInput();
		UpdateWorld();
		HandleOutput();

		//debug
//		running = false;
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	netUtil.Close();
	SDLNet_Quit();
}

void ServerApplication::HandleInput() {
	//accept new connections
	//accept updates from the clients
	//read the updates from the clients into internal containers
	PacketData packet;
	while(netUtil.Receive()) {
		memcpy(reinterpret_cast<void*>(&packet), netUtil.GetInData(), sizeof(PacketData));
		switch(packet.type) {
			case PacketList::PING:
				Ping(&packet);
			break;
			case PacketList::JOINREQUEST:
				JoinRequest(&packet);
			break;
			case PacketList::DISCONNECT:
				Disconnect(&packet);
			break;
			case PacketList::MOVEMENT:
				Movement(&packet);
			break;
		}
	} 
}

void ServerApplication::UpdateWorld() {
	//update internals ie.
	//	ai
	//	loot drops
	delta.Calculate();
	for (auto it : clientMap) {
		it.second.Update(delta.GetDelta());
	}
}

void ServerApplication::HandleOutput() {
	//send all information to new connections
	//selective updates to existing connectons
}

//-------------------------
//network commands
//-------------------------

void ServerApplication::Ping(PacketData* packet) {
	//respond to pings with the server name
	packet->type = PacketList::PONG;
	sprintf(packet->pong.metadata, "%s",configUtil.CString("servername"));
	netUtil.Send(&netUtil.GetInPacket()->address, reinterpret_cast<void*>(packet), sizeof(PacketData));
}

void ServerApplication::JoinRequest(PacketData* packet) {
	//TODO
	cout << "Join request..." << endl;
//	if (playerMgr.GetPlayerMap()->size() >= playerMgr.GetMaxPlayers()) {
//		//rejection
//		return;
//	}
//	int ch = netUtil.Bind(&netUtil.GetInPacket()->address, -1);
//	cout << ch << endl;
}

void ServerApplication::Disconnect(PacketData* packet) {
	//TODO
}

void ServerApplication::Movement(PacketData* packet) {
	//TODO
}
