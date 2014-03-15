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
	//TODO: flesh out this error if needed
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
	int ret = 0;

	//load config
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
	network.Open(config.Int("server.port"), sizeof(NetworkPacket));
	cout << "Initialized SDL_net" << endl;

	//Init SQL
	ret = sqlite3_open_v2(config["server.dbname"].c_str(), &database, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, nullptr);
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
	mapPager.SetRegionWidth(config.Int("map.pager.width"));
	mapPager.SetRegionHeight(config.Int("map.pager.height"));
	mapPager.SetRegionDepth(config.Int("map.pager.depth"));
	//TODO: pass args to the generator & format as needed
	//NOTE: I might need to rearrange the init process so that lua & SQL can interact
	//      with the map system as needed.
	cout << "Initialized the map system" << endl;

	//finalize the startup
	cout << "Startup completed successfully" << endl;
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
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	cout << "Shutting down" << endl;
	//empty the members
	//TODO: player manager
	//TODO: client manager

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
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, sizeof(NetworkPacket));
}

void ServerApplication::HandleJoinRequest(NetworkPacket packet) {
	//register the new client
	ClientEntry c;
	c.address = packet.meta.srcAddress;
	clientMap[clientCounter] = c;

	//send the client their info
	char buffer[sizeof(NetworkPacket)];

	packet.meta.type = NetworkPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.index = clientCounter;
	serialize(&packet, buffer);

	network.Send(&clientMap[clientCounter].address, buffer, sizeof(NetworkPacket));

	//finished this routine
	clientCounter++;
	cout << "connect, total: " << clientMap.size() << endl;
}

void ServerApplication::HandleDisconnect(NetworkPacket packet) {
	//disconnect the specified client
	//TODO: authenticate who is disconnecting/kicking
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	network.Send(&clientMap[packet.clientInfo.index].address, buffer, sizeof(NetworkPacket));
	clientMap.erase(packet.clientInfo.index);

	//delete players from all clients
	NetworkPacket delPacket;
	delPacket.meta.type = NetworkPacket::Type::PLAYER_DELETE;

	erase_if(playerMap, [&](std::pair<int, PlayerEntry> it) -> bool {
		//find the internal players to delete
		if (it.second.clientIndex == packet.clientInfo.index) {
			delPacket.playerInfo.playerIndex = it.first;
			//send the delete player command to all clients
			PumpPacket(delPacket);
			return true;
		}
		return false;
	});

	//finished this routine
	cout << "disconnect, total: " << clientMap.size() << endl;
}

void ServerApplication::HandleSynchronize(NetworkPacket packet) {
	//send all the server's data to this client
	//TODO: compensate for large distances
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

	//finished this routine
	cout << "shutting down" << endl;
}

void ServerApplication::HandlePlayerNew(NetworkPacket packet) {
	//create the new player object
	PlayerEntry newPlayer;
	newPlayer.clientIndex = packet.playerInfo.clientIndex;
	newPlayer.mapIndex = 0;
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
	erase_if(playerMap, [&](pair<int, PlayerEntry> it) -> bool {
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
	//I don't really like this, but it'll do for now
	char buffer[sizeof(NetworkPacket)];
	serialize(&packet, buffer);
	for (auto& it : clientMap) {
		network.Send(&it.second.address, buffer, sizeof(NetworkPacket));
	}
}
