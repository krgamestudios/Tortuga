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
		//handle input
		while(netUtil.Receive()) {
			PacketData packet;
			memcpy(reinterpret_cast<void*>(&packet), netUtil.GetInData(), sizeof(PacketData));
			switch(packet.type) {
//				case PacketList::NONE:
//					//
//				break;
				case PacketList::PING:
					Ping(&packet);
				break;
//				case PacketList::PONG:
//					//
//				break;
				case PacketList::JOINREQUEST:
					JoinRequest(&packet);
				break;
//				case PacketList::JOINCONFIRM:
//					//
//				break;
				case PacketList::DISCONNECT:
					Disconnect(packet.disconnect.playerID);
				break;
//				case PacketList::SYNCHRONIZE:
//					//
//				break;
//				case PacketList::NEWPLAYER:
//					//
//				break;
//				case PacketList::DELETEPLAYER:
//					//
//				break;
				case PacketList::MOVEMENT:
					Movement(&packet);
				break;
			}
		}
		//update the world
//TODO	Delta::Calculate();
//		for (auto it : clientMap) {
//			it.second.Update(Delta::GetTime());
//		}

		//handle output...
		//TODO...

		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	netUtil.Close();
	SDLNet_Quit();
}

//-------------------------
//network commands
//-------------------------

void ServerApplication::Ping(PacketData* packet) {
	//respond to pings with the server name
	if (!packet) {
		throw(runtime_error("Ping() received null"));
	}
	packet->type = PacketList::PONG;
	snprintf(packet->pong.metadata,PACKET_STRING_SIZE, "%s",configUtil.CString("servername"));
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

	//debug
#ifdef DEBUG
	cout << "New player:" << endl;
	cout << "\tplayerID: " << clientMap[playerID].playerID << endl;
	cout << "\tchannel: " << clientMap[playerID].channel << endl;
	cout << "\thandle: " << clientMap[playerID].handle << endl;
	cout << "\tavatar: " << clientMap[playerID].avatar << endl;
#else
	cout << "New player: " << playerID << endl;
#endif

	//join confirm
	PacketData p;
	p.type = PacketList::JOINCONFIRM;
	p.joinConfirm.playerID = clientMap[playerID].playerID;
	netUtil.Send(clientMap[playerID].channel, &p, sizeof(PacketData));

	//send it out to the clients
	NewClientData(playerID);

#ifdef DEBUG
	cout << "current players: " << clientMap.size() << endl;
#endif
}

void ServerApplication::Disconnect(int playerID) {
	if (clientMap.find(playerID) == clientMap.end()) {
		return;
	}
	cout << "disconnecting: " << playerID << endl;
	netUtil.Unbind(clientMap[playerID].channel);
	clientMap.erase(playerID);

	//TODO: Delete player

#ifdef DEBUG
	cout << "current players: " << clientMap.size() << endl;
#endif
}

void ServerApplication::Movement(PacketData* packet) {
	if (!packet) {
		throw(runtime_error("Movement() received null"));
	}
	clientMap[packet->movement.playerID].position = packet->movement.position;
	clientMap[packet->movement.playerID].motion = packet->movement.motion;
	//simple relay
	//TODO: SendClientData(packet->movement.playerID);
	for (auto it : clientMap) {
		netUtil.Send(it.second.channel, packet, sizeof(PacketData));
	}
}

void ServerApplication::NewClientData(int playerID) {
	if (clientMap.find(playerID) == clientMap.end()) {
		throw(runtime_error("NewClientData() Failed to find the client"));
	}
	//create the packet to send
	PacketData p;
	p.type = PacketList::NEWPLAYER;
	p.newPlayer.playerID = clientMap[playerID].playerID;
	snprintf(p.newPlayer.handle, PACKET_STRING_SIZE, "%s", clientMap[playerID].handle.c_str());
	snprintf(p.newPlayer.avatar, PACKET_STRING_SIZE, "%s", clientMap[playerID].avatar.c_str());
	p.newPlayer.position = clientMap[playerID].position;
	p.newPlayer.motion = clientMap[playerID].motion;

	//send the packet
	for (auto it : clientMap) {
		netUtil.Send(it.second.channel, &p, sizeof(PacketData));
	}
}

void ServerApplication::SendClientData(int playerID) {
	//TODO
}

void ServerApplication::SynchronizeClient(int playerID) {
	//TODO
}
