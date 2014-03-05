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

#include "utility.hpp"

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

//-------------------------
//Define the ServerApplication
//-------------------------

ServerApplication::ServerApplication() {
	//
}

ServerApplication::~ServerApplication() {
	//
}

void ServerApplication::Init(int argc, char** argv) {
	//TODO: proper command line option parsing

	//load config
	config.Load("rsc\\config.cfg");

	//Init SDL
	if (SDL_Init(0)) {
		throw(runtime_error("Failed to initialize SDL"));
	}
	cout << "initialized SDL" << endl;

	//Init SDL_net
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to init SDL_net"));
	}
	network.Open(config.Int("server.port"), sizeof(NetworkPacket));
	cout << "initialized SDL_net" << endl;

	//Init SQL
	string dbname = (config["server.dbname"].size()) ? config["server.dbname"] : std::string(argv[0]) + ".db"; //fancy and unnecessary
	int ret = sqlite3_open_v2(dbname.c_str(), &database, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, nullptr);
	if (ret != SQLITE_OK || !database) {
		throw(runtime_error("Failed to open the server database"));
	}
	cout << "initialized SQL" << endl;
	cout << "Database filename: " << dbname << endl;

	//TODO: move this into a function?
	//Run setup scripts
	ifstream is("rsc\\scripts\\setup_server.sql");
	if (!is.is_open()) {
		throw(runtime_error("Failed to run database setup script"));
	}
	string script;
	getline(is, script, '\0');
	is.close();
	sqlite3_exec(database, script.c_str(), nullptr, nullptr, nullptr);
}

void ServerApplication::Loop() {
	NetworkPacket packet;

	while(running) {
		//suck in the waiting packets & process them
		try {
			while(network.Receive()) {
				deserialize(&packet, network.GetInData());
				packet.meta.srcAddress = network.GetInPacket()->address;
				HandlePacket(packet);
			}
		}
		catch(exception& e) {
			cerr << "Network Error: " << e.what() << endl;
		}

		//give the computer a break
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	//members
	network.Close();

	//APIs
	sqlite3_close_v2(database);
	SDLNet_Quit();
	SDL_Quit();
}

void ServerApplication::HandlePacket(NetworkPacket packet) {
	switch(packet.meta.type) {
		case NetworkPacket::Type::BROADCAST_REQUEST:
			HandleBroadcastRequest(packet);
		break;

		case NetworkPacket::Type::JOIN_REQUEST:
			HandleJoinRequest(packet);
		break;

		case NetworkPacket::Type::DISCONNECT:
			HandleDisconnect(packet);
		break;

		case NetworkPacket::Type::SYNCHRONIZE:
			HandleSynchronize(packet);
		break;

		case NetworkPacket::Type::SHUTDOWN:
			HandleShutdown(packet);
		break;

		case NetworkPacket::Type::PLAYER_NEW:
			HandlePlayerNew(packet);
		break;

		case NetworkPacket::Type::PLAYER_DELETE:
			HandlePlayerDelete(packet);
		break;

		case NetworkPacket::Type::PLAYER_UPDATE:
			HandlePlayerUpdate(packet);
		break;

		//handle errors
		default:
			throw(runtime_error("Unknown NetworkPacket::Type encountered"));
		break;
	}
}

void ServerApplication::HandleBroadcastRequest(NetworkPacket packet) {
	//send back the server's name
	packet.meta.type = NetworkPacket::Type::BROADCAST_RESPONSE;
	snprintf(packet.serverInfo.name, PACKET_STRING_SIZE, "%s", config["server.name"].c_str());
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, sizeof(NetworkPacket));
}

void ServerApplication::HandleJoinRequest(NetworkPacket packet) {
	//TODO: prevent duplicate logins from the same address?

	//create the new client, filling it with the correct info
	Client newClient;
	newClient.address = packet.meta.srcAddress;

	//push the new client
	clientMap[clientCounter] = newClient;

	//send the client their info
	packet.meta.type = NetworkPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.index = clientCounter;
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	network.Send(&newClient.address, buffer, sizeof(NetworkPacket));

	//finished this routine
	clientCounter++;
	cout << "connect, total: " << clientMap.size() << endl;
}

void ServerApplication::HandleDisconnect(NetworkPacket packet) {
	//disconnect the specified client
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	network.Send(&clientMap[packet.clientInfo.index].address, buffer, sizeof(NetworkPacket));
	clientMap.erase(packet.clientInfo.index);

	//delete players
	erase_if(playerMap, [&](pair<int, Player> it) -> bool {
		if (it.second.clientIndex == packet.clientInfo.index) {
			NetworkPacket delPacket;

			//data to delete one specific player
			delPacket.meta.type = NetworkPacket::Type::PLAYER_DELETE;
			delPacket.playerInfo.playerIndex = it.first;

			//send to all
			PumpPacket(delPacket);

			return true;
		}
		return false;
	});

	cout << "disconnect, total: " << clientMap.size() << endl;
}

void ServerApplication::HandleSynchronize(NetworkPacket packet) {
	//send all the server's data to this client
	NetworkPacket newPacket;
	char buffer[sizeof(NetworkPacket)];

	//players
	newPacket.meta.type = NetworkPacket::Type::PLAYER_UPDATE;
	for (auto& it : playerMap) {
		newPacket.playerInfo.playerIndex = it.first;
		snprintf(newPacket.playerInfo.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(newPacket.playerInfo.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		newPacket.playerInfo.position = it.second.position;
		newPacket.playerInfo.motion = it.second.motion;
		serialize(&newPacket, buffer);
		network.Send(&clientMap[packet.clientInfo.index].address, buffer, sizeof(NetworkPacket));
	}
}

void ServerApplication::HandleShutdown(NetworkPacket packet) {
	//end the server
	running = false;

	//disconnect all clients
	packet.meta.type = NetworkPacket::Type::DISCONNECT;
	PumpPacket(packet);

	cout << "shutting down" << endl;
}

void ServerApplication::HandlePlayerNew(NetworkPacket packet) {
	//create the new player object
	Player newPlayer;
	newPlayer.clientIndex = packet.playerInfo.clientIndex;
	newPlayer.handle = packet.playerInfo.handle;
	newPlayer.avatar = packet.playerInfo.avatar;
	newPlayer.position = {0,0};
	newPlayer.motion = {0,0};

	//push this player
	playerMap[playerCounter] = newPlayer;

	//send the client their info
	packet.playerInfo.playerIndex = playerCounter;
	packet.playerInfo.position = playerMap[playerCounter].position;
	packet.playerInfo.motion = playerMap[playerCounter].motion;

	//actually send to everyone
	PumpPacket(packet);

	//finish this routine
	playerCounter++;
}

void ServerApplication::HandlePlayerDelete(NetworkPacket packet) {
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot delete a non-existant player"));
	}

	//delete players
	erase_if(playerMap, [&](pair<int, Player> it) -> bool {
		if (it.first == packet.playerInfo.playerIndex) {
			NetworkPacket delPacket;

			//data to delete one specific player
			delPacket.meta.type = NetworkPacket::Type::PLAYER_DELETE;
			delPacket.playerInfo.playerIndex = it.first;

			//send to all
			PumpPacket(delPacket);

			return true;
		}
		return false;
	});
}

void ServerApplication::HandlePlayerUpdate(NetworkPacket packet) {
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot update a non-existant player"));
	}

	//server is the slave to the clients, but only for now
	playerMap[packet.playerInfo.playerIndex].position = packet.playerInfo.position;
	playerMap[packet.playerInfo.playerIndex].motion = packet.playerInfo.motion;

	PumpPacket(packet);
}

void ServerApplication::PumpPacket(NetworkPacket packet) {
	//send this packet to all clients
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	for (auto& it : clientMap) {
		network.Send(&it.second.address, buffer, sizeof(NetworkPacket));
	}
}