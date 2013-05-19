#include "server.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

void Server::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialize SDL_net"));
	}
	configUtil.Load("config.cfg");
	netUtil.Open(configUtil.Integer("port"), sizeof(Packet));
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
				//respond to pings with the server name
				cout << "responding to ping..." << endl;
				packet.type = PacketList::PONG;
				sprintf(packet.pong.buffer, "%s",configUtil.CString("servername"));
				netUtil.Send(&netUtil.GetInPacket()->address, reinterpret_cast<void*>(&packet), sizeof(Packet));
			break;
			case PacketList::JOINREQUEST:
				//
			break;
			case PacketList::NEWPLAYER:
				//
			break;
			case PacketList::DELETEPLAYER:
				//
			break;
			case PacketList::MOTIONUPDATE:
				//
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
