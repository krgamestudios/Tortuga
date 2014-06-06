/* Copyright: (c) Kayne Ruse 2014
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

#include "sql_utility.hpp"
#include "serial.hpp"

#include <stdexcept>
#include <iostream>
#include <string>

//-------------------------
//Define the public members
//-------------------------

void ServerApplication::Init(int argc, char** argv) {
	//NOTE: I might need to rearrange the init process so that lua & SQL can interact with the map system as needed.
	std::cout << "Beginning startup" << std::endl;

	//initial setup
	config.Load("rsc\\config.cfg");

	//-------------------------
	//Initialize the APIs
	//-------------------------

	//Init SDL
	if (SDL_Init(0)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	std::cout << "Initialized SDL" << std::endl;

	//Init SDL_net
	if (SDLNet_Init()) {
		throw(std::runtime_error("Failed to initialize SDL_net"));
	}
	network.Open(config.Int("server.port"));
	std::cout << "Initialized SDL_net" << std::endl;

	//Init SQL
	int ret = sqlite3_open_v2(config["server.dbname"].c_str(), &database, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, nullptr);
	if (ret != SQLITE_OK || !database) {
		throw(std::runtime_error(std::string() + "Failed to initialize SQL: " + sqlite3_errmsg(database) ));
	}
	std::cout << "Initialized SQL" << std::endl;

	//Init lua
	luaState = luaL_newstate();
	if (!luaState) {
		throw(std::runtime_error("Failed to initialize lua"));
	}
	luaL_openlibs(luaState);
	std::cout << "Initialized lua" << std::endl;

	//-------------------------
	//Setup the objects
	//-------------------------

	//setup the map object
	regionPager.GetAllocator()->SetLuaState(luaState);
	regionPager.GetFormat()->SetLuaState(luaState);
	regionPager.GetFormat()->SetSaveDir(config["dir.maps"] + config["map.savename"]);
	std::cout << "Prepared the map system" << std::endl;

	//push the pager onto the lua registry
	lua_pushstring(luaState, "pager");
	lua_pushlightuserdata(luaState, reinterpret_cast<void*>(&regionPager));
	lua_settable(luaState, LUA_REGISTRYINDEX);
	std::cout << "Registered the map system in lua" << std::endl;

	//-------------------------
	//Run the startup scripts
	//-------------------------

	//setup the database
	if (runSQLScript(database, config["dir.scripts"] + "setup_server.sql")) {
		throw(std::runtime_error("Failed to initialize SQL's setup script"));
	}
	std::cout << "Completed SQL's setup script" << std::endl;

	//run lua's startup script
	if (luaL_dofile(luaState, (config["dir.scripts"] + "setup_server.lua").c_str())) {
		throw(std::runtime_error(std::string() + "Failed to initialize lua's setup script: " + lua_tostring(luaState, -1) ));
	}
	std::cout << "Completed lua's setup script" << std::endl;

	//debug output
	std::cout << "Internal sizes:" << std::endl;
	std::cout << "\tsizeof(SerialPacket): " << sizeof(SerialPacket) << std::endl;
	std::cout << "\tPACKET_BUFFER_SIZE: " << PACKET_BUFFER_SIZE << std::endl;

	//finalize the startup
	std::cout << "Startup completed successfully" << std::endl;

	//debugging
	//
}

void ServerApplication::Proc() {
	SerialPacket packet;
	while(running) {
		//suck in the waiting packets & process them
		while(network.Receive(&packet)) {
			HandlePacket(packet);
		}
		//update the internals
		//TODO: update the internals i.e. player positions
		//give the computer a break
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	std::cout << "Shutting down" << std::endl;

	//save the server state
	for (auto& it : accountMap) {
		SaveUserAccount(it.first);
	}
	for (auto& it : characterMap) {
		SaveCharacter(it.first);
	}

	//empty the members
	accountMap.clear();
	characterMap.clear();
	regionPager.UnloadAll();

	//APIs
	lua_close(luaState);
	sqlite3_close_v2(database);
	network.Close();
	SDLNet_Quit();
	SDL_Quit();

	std::cout << "Shutdown finished" << std::endl;
}

//-------------------------
//Define the network switch
//-------------------------

void ServerApplication::HandlePacket(SerialPacket packet) {
	switch(packet.meta.type) {
		//basic connections
		case SerialPacketType::BROADCAST_REQUEST:
			HandleBroadcastRequest(packet);
		break;
		case SerialPacketType::JOIN_REQUEST:
			HandleJoinRequest(packet);
		break;
		case SerialPacketType::DISCONNECT:
			HandleDisconnect(packet);
		break;
		case SerialPacketType::SHUTDOWN:
			HandleShutdown(packet);
		break;

		//map management
		case SerialPacketType::REGION_REQUEST:
			HandleRegionRequest(packet);
		break;

		//combat management
		//TODO: combat management

		//character management
		case SerialPacketType::CHARACTER_NEW:
			HandleCharacterNew(packet);
		break;
		case SerialPacketType::CHARACTER_DELETE:
			HandleCharacterDelete(packet);
		break;
		case SerialPacketType::CHARACTER_UPDATE:
			HandleCharacterUpdate(packet);
		break;
		case SerialPacketType::CHARACTER_STATS_REQUEST:
			HandleCharacterUpdate(packet);
		break;

		//enemy management
		//TODO: enemy management

		//mismanagement
		case SerialPacketType::SYNCHRONIZE:
			HandleSynchronize(packet);
		break;

		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacketType encountered in the server: " + to_string_custom(int(packet.type))));
		break;
	}
}

//-------------------------
//Define the network handlers
//-------------------------

void ServerApplication::HandleBroadcastRequest(SerialPacket packet) {
	//pack the server's data
	packet.meta.type = SerialPacket::Type::BROADCAST_RESPONSE;
	packet.serverInfo.networkVersion = NETWORK_VERSION;
	snprintf(packet.serverInfo.name, PACKET_STRING_SIZE, "%s", config["server.name"].c_str());
	packet.serverInfo.playerCount = characterMap.size();

	//bounce this packet
	network.SendTo(&packet.meta.srcAddress, &packet);
}

void ServerApplication::HandleJoinRequest(SerialPacket packet) {
	//create the new client
	ClientData newClient;
	newClient.address = packet.meta.srcAddress;

	//load the user account
	int accountIndex = LoadUserAccount(packet.clientInfo.username, clientUID);
	if (accountIndex < 0) {
		//TODO: send rejection packet
		std::cerr << "Error: Account already loaded: " << accountIndex << std::endl;
		return;
	}

	//load the new character
	int characterIndex = LoadCharacter(accountIndex, packet.clientInfo.handle, packet.clientInfo.avatar);
	if (characterIndex < 0) {
		//TODO: send rejection packet
		std::cerr << "Error: Character already loaded: " << characterIndex << std::endl;
		UnloadUserAccount(accountIndex);
		return;
	}

	//send the client their info
	packet.meta.type = SerialPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.clientIndex = clientUID;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	//bounce this packet
	network.SendTo(&newClient.address, &packet);

	//reference to prevent multiple lookups
	//TODO: I need a way to pack structures unto packets more easily
	//NOTE: this chunk of code is similar to HandleSynchronize
	CharacterData& character = characterMap[characterIndex];

	//send the new character to all clients
	packet.meta.type = SerialPacket::Type::CHARACTER_NEW;
	packet.characterInfo.characterIndex = characterIndex;
	strncpy(packet.characterInfo.handle, character.handle.c_str(), PACKET_STRING_SIZE);
	strncpy(packet.characterInfo.avatar, character.avatar.c_str(), PACKET_STRING_SIZE);
	packet.characterInfo.mapIndex = character.mapIndex;
	packet.characterInfo.origin = character.origin;
	packet.characterInfo.motion = character.motion;
	packet.characterInfo.stats = character.stats;

	PumpPacket(packet);

	//TODO: don't send anything to a certain client until they send the OK (the sync packet? or ignore client side?)
	//finished this routine
	clientMap[clientUID++] = newClient;
	std::cout << "Connect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleSynchronize(SerialPacket packet) {
	//TODO: compensate for large distances

	//send all the server's data to this client
	SerialPacket newPacket;

	//characters
	newPacket.meta.type = SerialPacket::Type::CHARACTER_UPDATE;
	for (auto& it : characterMap) {
		//TODO: update this for the expanded CharacterData structure
		newPacket.characterInfo.characterIndex = it.first;
		snprintf(newPacket.characterInfo.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(newPacket.characterInfo.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		newPacket.characterInfo.mapIndex = it.second.mapIndex;
		newPacket.characterInfo.origin = it.second.origin;
		newPacket.characterInfo.motion = it.second.motion;
		newPacket.characterInfo.stats = it.second.stats;

		network.SendTo(&clientMap[packet.clientInfo.clientIndex].address, &newPacket);
	}
}

void ServerApplication::HandleDisconnect(SerialPacket packet) {
	//TODO: authenticate who is disconnecting/kicking

	//forward to the specified client
	network.SendTo(&clientMap[accountMap[packet.clientInfo.accountIndex].clientIndex].address, &packet);

	//unload client and server-side characters
	for (std::map<int, CharacterData>::iterator it = characterMap.begin(); it != characterMap.end(); /* EMPTY */ ) {
		if (it->second.owner == packet.clientInfo.accountIndex) {
			PumpCharacterUnload(it->first);
			SaveCharacter(it->first);
			it = characterMap.erase(it); //efficient
			continue;
		}
		else {
			++it;
		}
	}

	//erase the in-memory stuff
	clientMap.erase(accountMap[packet.clientInfo.accountIndex].clientIndex);
	UnloadUserAccount(packet.clientInfo.accountIndex);

	//finished this routine
	std::cout << "Disconnect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleShutdown(SerialPacket packet) {
	//TODO: authenticate who is shutting the server down

	//end the server
	running = false;

	//disconnect all clients
	packet.meta.type = SerialPacket::Type::DISCONNECT;
	PumpPacket(packet);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

void ServerApplication::HandleCharacterUpdate(SerialPacket packet) {
	//TODO: this should be moved elsewhere
	if (characterMap.find(packet.characterInfo.characterIndex) == characterMap.end()) {
		throw(std::runtime_error("Cannot update a non-existant character"));
	}

	//TODO: the server needs it's own movement system too
	characterMap[packet.characterInfo.characterIndex].origin = packet.characterInfo.origin;
	characterMap[packet.characterInfo.characterIndex].motion = packet.characterInfo.motion;

	PumpPacket(packet);
}

void ServerApplication::HandleRegionRequest(SerialPacket packet) {
	//TODO: this should be moved elsewhere
	packet.meta.type = SerialPacket::Type::REGION_CONTENT;
	packet.regionInfo.region = regionPager.GetRegion(packet.regionInfo.x, packet.regionInfo.y);

	//send the content
	network.SendTo(&packet.meta.srcAddress, &packet);
}

void ServerApplication::PumpPacket(SerialPacket packet) {
	//NOTE: I don't really like this, but it'll do for now
	for (auto& it : clientMap) {
		network.SendTo(&it.second.address, &packet);
	}
}

void ServerApplication::PumpCharacterUnload(int uid) {
	//delete the client-side character(s)
	SerialPacket delPacket;
	delPacket.meta.type = SerialPacket::Type::CHARACTER_DELETE;
	delPacket.characterInfo.characterIndex = uid;
	PumpPacket(delPacket);
}

//-------------------------
//Define the utility methods
//-------------------------

//TODO: utility methods

