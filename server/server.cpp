#include "server.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

void Server::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialize SDL_net"));
	}
	configUtil.Load("config.cfg");
	netUtil.Open(configUtil.Integer("server.port"), sizeof(Packet));
	running = true;
}

void Server::Proc() {
	while(running) {
		HandleInput();
		UpdateWorld();
		HandleOutput();

		//debug
//		running = false;
	}
}

void Server::Quit() {
	netUtil.Close();
	SDLNet_Quit();
}

void Server::HandleInput() {
	//accept new connections
	//accept updates from the clients
	//read the updates from the clients into internal containers
	Packet packet;
	while(netUtil.Receive()) {
		memcpy(reinterpret_cast<void*>(&packet), netUtil.GetInData(), sizeof(Packet));
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

void Server::UpdateWorld() {
	//update internals ie.
	//	ai
	//	loot drops
	delta.Calculate();
	playerMgr.UpdateAll(delta.GetDelta());
}

void Server::HandleOutput() {
	//send all information to new connections
	//selective updates to existing connectons
}

//-------------------------
//network commands
//-------------------------

void Server::Ping(Packet* packet) {
	//respond to pings with the server name
	cout << "responding to ping..." << endl;
	packet->type = PacketList::PONG;
	sprintf(packet->pong.serverName, "%s",configUtil.CString("servername"));
	netUtil.Send(&netUtil.GetInPacket()->address, reinterpret_cast<void*>(packet), sizeof(Packet));
}

void Server::JoinRequest(Packet* packet) {
	//
}

void Server::Disconnect(Packet* packet) {
	//
}

void Server::Movement(Packet* packet) {
	//
}
