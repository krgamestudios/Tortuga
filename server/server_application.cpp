/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#include "server_application.hpp"

#include <stdexcept>
#include <iostream>
#include <chrono>

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

/* ServerApplication::Init()
 * This function initializes the entire program. There are a number of things
 * that could go wrong here, which is why there is such an unusual order of
 * operations.
 * Important things to note:
 *   The APIs are initiated here.
 *   The global objects are initialized here.
 *   The ConfigUtility's call to Load() also ensures that the "rsc\" folder is in the directory. It's easy to forget it.
*/

void ServerApplication::Init() {
	//load the config file
	try {
		configUtil->Load("rsc/config.cfg");
	}
	catch(std::runtime_error& e) {
		std::string s = e.what();
		s += "; Ensure that the \"rsc\" directory is present";
		throw(std::runtime_error(s));
	}

	//check the port is valid
	if (configUtil->Int("server.port") <= 0) {
		throw(runtime_error("Cannot open the server on an invalid port or port 0"));
	}

	//initialize the APIs
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialize SDL_net"));
	}

	//initiate the remaining singletons
	netUtil->Open(configUtil->Int("server.port"), sizeof(Packet::Packet));

	//create the threads
	beginQueueThread();

	//output the server information
	cout << configUtil->String("server.name") << endl;
	cout << "Open on port " << configUtil->String("server.port") << endl;

	//disable this for debugging
	running = true;
}

void ServerApplication::Proc() {
	typedef chrono::high_resolution_clock Clock;

	Clock::time_point lastTick = Clock::now();
	Clock::duration delta;

	while(running) {
		try {
			//process all packets on the network queue
			while(HandlePacket(popNetworkPacket()));
		}
		catch(exception& e) {
			//handle any errors
			cerr << "Network Error: " << e.what() << endl;
			continue;
		}

		//get the time since last update
		delta = Clock::now() - lastTick;
		lastTick = Clock::now();

		//update the world
		UpdateWorld(double(delta.count()) / Clock::duration::period::den);

		//give the machine a break
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	//close the threads
	endQueueThread();

	//clean up the singletons
	netUtil->Close();

	//deinitialize the APIs
	SDLNet_Quit();
}

//-------------------------
//Game loop
//-------------------------

void ServerApplication::UpdateWorld(double delta) {
	//the recalc here each loop is a stopgap, see issue #9 for details
	for (auto& it : players) {
		if (it.second.motion.x != 0 && it.second.motion.y != 0) {
			constexpr double d = 1.0/sqrt(2);
			it.second.position += it.second.motion * delta * d;
		}
		else {
			it.second.position += it.second.motion * delta;
		}
	}
}

//-------------------------
//Network loop
//-------------------------

int ServerApplication::HandlePacket(Packet::Packet p) {
	switch(p.meta.type) {
		case Packet::Type::NONE:
			//DO NOTHING
			return 0;
		break;
		case Packet::Type::PING:
			//quick pong
			p.meta.type = Packet::Type::PONG;
			netUtil->Send(&p.meta.address, &p, sizeof(Packet::Packet));
		break;
		case Packet::Type::PONG:
			//
		break;
		case Packet::Type::BROADCAST_REQUEST:
			HandleBroadcast(p.broadcastRequest);
		break;
//		case PacketType::BROADCAST_RESPONSE:
//			//
//		break;
		case Packet::Type::JOIN_REQUEST:
			HandleConnection(p.joinRequest);
		break;
//		case PacketType::JOIN_RESPONSE:
//			//
//		break;
		case Packet::Type::DISCONNECT:
			HandleDisconnection(p.disconnect);
		break;
		case Packet::Type::SYNCHRONIZE:
			SynchronizeEverything(p.synchronize);
		break;
		case Packet::Type::PLAYER_NEW:
			AddPlayer(p.playerData);
			RelayPacket(p);
		break;
		case Packet::Type::PLAYER_DELETE:
			RemovePlayer(p.playerDelete);
			RelayPacket(p);
		break;
		case Packet::Type::PLAYER_UPDATE:
			UpdatePlayer(p.playerData);
			RelayPacket(p);
		break;
		default:
			throw(runtime_error("Failed to recognize the packet type: " + itos(int(p.meta.type))));
	}
	return 1;
}

void ServerApplication::RelayPacket(Packet::Packet& p) {
	//pump this packet to all clients
	for (auto& it : clients) {
		netUtil->Send(&it.second.address, &p, sizeof(Packet::Packet));
	}
}

void ServerApplication::SynchronizeEverything(Packet::Synchronize& sync) {
	//send all known data to this client
	//TODO multithreading?

	//all players
	Packet::Packet p;
	p.meta.type = Packet::Type::PLAYER_UPDATE;
	for (auto& it : players) {
		p.playerData.playerIndex = it.second.index;
		p.playerData.clientIndex = it.second.clientIndex;
		snprintf(p.playerData.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(p.playerData.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		p.playerData.position = it.second.position;
		p.playerData.motion = it.second.motion;

		netUtil->Send(&clients[sync.clientIndex].address, &p, sizeof(Packet::Packet));
	}
}

void ServerApplication::HandleBroadcast(Packet::BroadcastRequest& bcast) {
	//respond to a broadcast request with the server's data
	Packet::Packet p;
	p.meta.type = Packet::Type::BROADCAST_RESPONSE;
	snprintf(p.broadcastResponse.name, PACKET_STRING_SIZE, "%s", configUtil->CString("server.name"));
	//TODO version information
	netUtil->Send(&bcast.meta.address, &p, sizeof(Packet::Packet));
}

void ServerApplication::HandleConnection(Packet::JoinRequest& request) {
	//create the entries
	ClientEntry newClient = {
		uniqueIndex++,
		request.meta.address
	};

	//push this information
	clients[newClient.index] = newClient;

	//send the player their information
	Packet::Packet p;
	p.meta.type = Packet::Type::JOIN_RESPONSE;
	p.joinResponse.clientIndex = newClient.index;

	//TODO: resource list

	netUtil->Send(&newClient.address, &p, sizeof(Packet::Packet));

	//debugging
	cout << "New connection: index " << newClient.index << endl;
	cout << "number of clients: " << clients.size() << endl;
}

void ServerApplication::HandleDisconnection(Packet::Disconnect& disconnect) {
	//disconnect a client (redundant message)
	netUtil->Send(&clients[disconnect.clientIndex].address, &disconnect, sizeof(Packet::Packet));
	clients.erase(disconnect.clientIndex);

	//TODO remove the player...
	//remove if(...)

	//debugging
	cout << "Lost connection: index " << disconnect.clientIndex << endl;
	cout << "number of clients: " << clients.size() << endl;
}

void ServerApplication::AddPlayer(Packet::PlayerData& p) {
	//add the player
	PlayerEntry newPlayer = {
		uniqueIndex++,
		p.clientIndex,
		p.handle,
		p.avatar,
		p.position,
		p.motion
	};

	players[newPlayer.index] = newPlayer;

	//prep for relay
	p.playerIndex = newPlayer.index;

	//debugging
	cout << "New player " << newPlayer.handle << "Has joined the game" << endl;
	cout << "Number of players: " << players.size() << endl;
}

void ServerApplication::RemovePlayer(Packet::PlayerDelete& p) {
	//TODO remove a player
}

void ServerApplication::UpdatePlayer(Packet::PlayerData& p) {
	//TODO update a player
}
