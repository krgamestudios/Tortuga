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

int runSQLScript(sqlite3* db, std::string fname) {
	ifstream is(fname);
	if (!is.is_open()) {
		return -1;
	}
	string script;
	getline(is, script, '\0');
	is.close();
	//NOTE: flesh out this error if needed
	if (sqlite3_exec(db, script.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		return -2;
	}
	return 0;
}

//-------------------------
//Define the public members
//-------------------------

void ServerApplication::Init(int argc, char** argv) {
	cout << "Beginning startup" << endl;

	//initial setup
	ClientEntry::uidCounter = 0;
	PlayerEntry::uidCounter = 0;
	config.Load("rsc\\config.cfg");

	//Init SDL
	if (SDL_Init(0)) {
		throw(runtime_error("Failed to initialize SDL"));
	}
	cout << "Initialized SDL" << endl;

	//Init SDL_net
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialize SDL_net"));
	}
	network.Open(config.Int("server.port"), PACKET_BUFFER_SIZE);
	cout << "Initialized SDL_net" << endl;

	//Init SQL
	int ret = sqlite3_open_v2(config["server.dbname"].c_str(), &database, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, nullptr);
	if (ret != SQLITE_OK || !database) {
		throw(runtime_error(string() + "Failed to initialize SQL: " + sqlite3_errmsg(database) ));
	}
	cout << "Initialized SQL" << endl;

	//setup the database
	if (runSQLScript(database, config["dir.scripts"] + "setup_server.sql")) {
		throw(runtime_error("Failed to initialize SQL's setup script"));
	}
	cout << "Initialized SQL's setup script" << endl;

	//lua
	luaState = luaL_newstate();
	if (!luaState) {
		throw(runtime_error("Failed to initialize lua"));
	}
	luaL_openlibs(luaState);
	cout << "Initialized lua" << endl;

	//run the startup script
	if (luaL_dofile(luaState, (config["dir.scripts"] + "setup_server.lua").c_str())) {
		throw(runtime_error(string() + "Failed to initialize lua's setup script: " + lua_tostring(luaState, -1) ));
	}
	cout << "Initialized lua's setup script" << endl;

	//setup the map object
	regionPager.GetAllocator()->SetLuaState(luaState);
	regionPager.GetFormat()->SetLuaState(luaState);
	//TODO: config parameter
	regionPager.GetFormat()->SetSaveDir("save/mapname/");
	//TODO: pass args to the generator & format as needed
	//NOTE: I might need to rearrange the init process so that lua & SQL can interact
	//      with the map system as needed.
	cout << "Initialized the map system" << endl;
	cout << "\tsizeof(NetworkPacket): " << sizeof(NetworkPacket) << endl;
	cout << "\tPACKET_BUFFER_SIZE: " << PACKET_BUFFER_SIZE << endl;

	//finalize the startup
	cout << "Startup completed successfully" << endl;

	//debugging
	//
}

void ServerApplication::Loop() {
	NetworkPacket packet;
	while(running) {
		//suck in the waiting packets & process them
		while(network.Receive()) {
			//get the packet
			deserialize(&packet, network.GetInData());
			//cache the source address
			packet.meta.srcAddress = network.GetInPacket()->address;
			//we need to go deeper
			HandlePacket(packet);
		}
		//give the computer a break
		//TODO: remove this?
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	cout << "Shutting down" << endl;
	//empty the members
	regionPager.UnloadAll();

	//APIs
	lua_close(luaState);
	sqlite3_close_v2(database);
	network.Close();
	SDLNet_Quit();
	SDL_Quit();
	cout << "Shutdown finished" << endl;
}

//-------------------------
//Define the uber switch
//-------------------------

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
		case NetworkPacket::Type::REGION_REQUEST:
			HandleRegionRequest(packet);
		break;
		//handle errors
		default:
			throw(runtime_error("Unknown NetworkPacket::Type encountered"));
		break;
	}
}

//-------------------------
//Handle various network input
//-------------------------

void ServerApplication::HandleBroadcastRequest(NetworkPacket packet) {
	//send back the server's metadata
	packet.meta.type = NetworkPacket::Type::BROADCAST_RESPONSE;
	//TODO: version info
	snprintf(packet.serverInfo.name, PACKET_STRING_SIZE, "%s", config["server.name"].c_str());
	//TODO: player count
	//TODO: map format
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, PACKET_BUFFER_SIZE);
}

void ServerApplication::HandleJoinRequest(NetworkPacket packet) {
	//register the new client
	ClientEntry newClient;
	newClient.address = packet.meta.srcAddress;
	clientMap[ClientEntry::uidCounter] = newClient;

	//send the client their index
	char buffer[PACKET_BUFFER_SIZE];
	packet.meta.type = NetworkPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.index = ClientEntry::uidCounter;
	serialize(&packet, buffer);

	//bounce this packet
	network.Send(&newClient.address, buffer, PACKET_BUFFER_SIZE);

	//finished this routine
	ClientEntry::uidCounter++;
	cout << "Connect, total: " << clientMap.size() << endl;
}

void ServerApplication::HandleDisconnect(NetworkPacket packet) {
	//TODO: authenticate who is disconnecting/kicking

	//disconnect the specified client
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&clientMap[packet.clientInfo.index].address, buffer, PACKET_BUFFER_SIZE);
	clientMap.erase(packet.clientInfo.index);

	//prep the delete packet
	NetworkPacket delPacket;
	delPacket.meta.type = NetworkPacket::Type::PLAYER_DELETE;

	//TODO: can this use DeletePlayer() instead?
	//delete server and client side players
	erase_if(playerMap, [&](std::pair<unsigned int, PlayerEntry> it) -> bool {
		//find the internal players to delete
		if (it.second.clientIndex == packet.clientInfo.index) {
			//send the delete player command to all clients
			delPacket.playerInfo.playerIndex = it.first;
			PumpPacket(delPacket);

			//delete this player object
			return true;
		}

		//don't delete this player object
		return false;
	});

	//finished this routine
	cout << "Disconnect, total: " << clientMap.size() << endl;
}

void ServerApplication::HandleSynchronize(NetworkPacket packet) {
	//TODO: compensate for large distances

	//send all the server's data to this client
	NetworkPacket newPacket;
	char buffer[PACKET_BUFFER_SIZE];

	//TODO: map?

	//players
	newPacket.meta.type = NetworkPacket::Type::PLAYER_UPDATE;
	for (auto& it : playerMap) {
		//TODO: update this for the expanded PlayerEntry structure
		newPacket.playerInfo.playerIndex = it.first;
		snprintf(newPacket.playerInfo.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(newPacket.playerInfo.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		newPacket.playerInfo.position = it.second.position;
		newPacket.playerInfo.motion = it.second.motion;
		serialize(&newPacket, buffer);
		network.Send(&clientMap[packet.clientInfo.index].address, buffer, PACKET_BUFFER_SIZE);
	}
}

void ServerApplication::HandleShutdown(NetworkPacket packet) {
	//end the server
	running = false;

	//disconnect all clients
	packet.meta.type = NetworkPacket::Type::DISCONNECT;
	PumpPacket(packet);

	//finished this routine
	cout << "Shutdown signal accepted" << endl;
}

void ServerApplication::HandlePlayerNew(NetworkPacket packet) {
	//register the new PlayerEntry
	//NOTE: assigning each field one-by-one so adding or moving a field doesn't break this code
	PlayerEntry newPlayer;

	//metadata
	newPlayer.clientIndex = packet.playerInfo.clientIndex;
	newPlayer.handle = packet.playerInfo.handle;
	newPlayer.avatar = packet.playerInfo.avatar;

	//position
	newPlayer.mapIndex = 0;
	newPlayer.position = {0,0};
	newPlayer.motion = {0,0};
	newPlayer.bbox = {0, 0, 0, 0};

	//stats
	//TODO

	//push this player
	playerMap[PlayerEntry::uidCounter] = newPlayer;

	//send the client their info
	packet.playerInfo.playerIndex = PlayerEntry::uidCounter;
	packet.playerInfo.position = newPlayer.position;
	packet.playerInfo.motion = newPlayer.motion;

	//actually send to everyone
	PumpPacket(packet);

	//finish this routine
	PlayerEntry::uidCounter++;
}

void ServerApplication::HandlePlayerDelete(NetworkPacket packet) {
	//TODO: remove this?
	//TODO: authenticate who is deleting this player
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot delete a non-existant player"));
	}

	//prep the delete packet
	NetworkPacket delPacket;
	delPacket.meta.type = NetworkPacket::Type::PLAYER_DELETE;

	//delete the specified playerEntry
	erase_if(playerMap, [&](std::pair<unsigned int, PlayerEntry> it) -> bool {
		//find the specified PlayerEntry
		if (it.first == packet.playerInfo.playerIndex) {
			//send to all
			delPacket.playerInfo.playerIndex = it.first;
			PumpPacket(delPacket);

			//delete this player
			return true;
		}
		//skip this player
		return false;
	});
}

void ServerApplication::HandlePlayerUpdate(NetworkPacket packet) {
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot update a non-existant player"));
	}

	//TODO: the server needs it's own movement system too
	playerMap[packet.playerInfo.playerIndex].position = packet.playerInfo.position;
	playerMap[packet.playerInfo.playerIndex].motion = packet.playerInfo.motion;

	PumpPacket(packet);
}

void ServerApplication::HandleRegionRequest(NetworkPacket packet) {
	char buffer[PACKET_BUFFER_SIZE];
	packet.meta.type = NetworkPacket::Type::REGION_CONTENT;
	packet.regionInfo.region = regionPager.GetRegion(packet.regionInfo.x, packet.regionInfo.y);
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, PACKET_BUFFER_SIZE);
}

void ServerApplication::PumpPacket(NetworkPacket packet) {
	//I don't really like this, but it'll do for now
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	for (auto& it : clientMap) {
		network.Send(&it.second.address, buffer, PACKET_BUFFER_SIZE);
	}
}
