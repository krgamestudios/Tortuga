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
				Disconnect(packet.disconnect.playerID);
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
	if (!packet) {
		return;
	}
	//respond to pings with the server name
	packet->type = PacketList::PONG;
	sprintf(packet->pong.metadata, "%s",configUtil.CString("servername"));
	netUtil.Send(&netUtil.GetInPacket()->address, reinterpret_cast<void*>(packet), sizeof(PacketData));
}

void ServerApplication::JoinRequest(PacketData* packet) {
	if (!packet || clientMap.size() >= maxClients) {
		//rejection
		return;
	}
	//add the player
	int playerID = uniqueIndex++;
	clientMap[playerID].playerID = playerID;
	clientMap[playerID].channel = netUtil.Bind(&netUtil.GetInPacket()->address, -1);
	clientMap[playerID].handle = packet->joinRequest.handle;
	clientMap[playerID].avatar = packet->joinRequest.avatar;
	cout << "New player: " << playerID << endl;

	//debug
#ifdef DEBUG
	cout << "playerID: " << clientMap[playerID].playerID << endl;
	cout << "channel: " << clientMap[playerID].channel << endl;
	cout << "handle: " << clientMap[playerID].handle << endl;
	cout << "avatar: " << clientMap[playerID].avatar << endl;
#endif

	//join confirm
	PacketData p;
	p.type = PacketList::JOINCONFIRM;
	p.joinConfirm.playerID = clientMap[playerID].playerID;
	netUtil.Send(clientMap[playerID].channel, &p, sizeof(PacketData));
}

void ServerApplication::Disconnect(int playerID) {
	//TODO: Delete player
	if (clientMap.find(playerID) == clientMap.end()) {
		return;
	}
	cout << "disconnecting: " << playerID << endl;
	netUtil.Unbind(clientMap[playerID].channel);
	clientMap.erase(playerID);
#ifdef DEBUG
	cout << "current players: " << clientMap.size() << endl;
#endif
}

void ServerApplication::Movement(PacketData* packet) {
	if (!packet) {
		return;
	}
	clientMap[packet->movement.playerID].position = packet->movement.position;
	clientMap[packet->movement.playerID].motion = packet->movement.motion;
	//simple relay
	for (auto it : clientMap) {
		netUtil.Send(it.second.channel, packet, sizeof(PacketData));
	}
}
