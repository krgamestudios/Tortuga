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
//public methods
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

	accountMgr.SetDatabase(database);
	characterMgr.SetDatabase(database);

	combatMgr.SetLuaState(luaState);
	roomMgr.SetLuaState(luaState);
	enemyMgr.SetLuaState(luaState);

	std::cout << "Internal managers ready" << std::endl;

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

	//-------------------------
	//debug output
	//-------------------------

	std::cout << "Internal sizes:" << std::endl;
	std::cout << "\tTile Size: " << sizeof(Region::type_t) << std::endl;
	std::cout << "\tRegion Format: " << REGION_WIDTH << ", " << REGION_HEIGHT << ", " << REGION_DEPTH << std::endl;
	std::cout << "\tRegion Content Footprint: " << REGION_WIDTH * REGION_HEIGHT * REGION_DEPTH * sizeof(Region::type_t) << std::endl;
	std::cout << "\tPACKET_BUFFER_SIZE (max size): " << PACKET_BUFFER_SIZE << std::endl;

	//-------------------------
	//finalize the startup
	//-------------------------

	std::cout << "Startup completed successfully" << std::endl;

	//-------------------------
	//debugging
	//-------------------------

	std::cout << "Debugging dump:" << std::endl;
	std::cout << "\tMAX_PACKET_SIZE:\t\t" << MAX_PACKET_SIZE << std::endl;
	std::cout << "\tsizeof(SerialPacket):\t\t" << sizeof(SerialPacket) << std::endl;
	std::cout << "\tsizeof(CharacterPacket):\t" << sizeof(CharacterPacket) << std::endl;
	std::cout << "\t\tsizeof(Statistics):\t" << sizeof(Statistics) << std::endl;
	std::cout << "\tsizeof(ClientPacket):\t\t" << sizeof(ClientPacket) << std::endl;
	std::cout << "\tsizeof(CombatPacket):\t\t" << sizeof(CombatPacket) << std::endl;
	std::cout << "\tsizeof(EnemyPacket):\t\t" << sizeof(EnemyPacket) << std::endl;
	std::cout << "\tsizeof(RegionPacket):\t\t" << sizeof(RegionPacket) << std::endl;
	std::cout << "\tsizeof(ServerPacket):\t\t" << sizeof(ServerPacket) << std::endl;
}

void ServerApplication::Proc() {
	char packetBuffer[MAX_PACKET_SIZE];
	while(running) {
		//suck in the waiting packets & process them
		while(network.Receive(reinterpret_cast<SerialPacket*>(packetBuffer))) {
			HandlePacket(reinterpret_cast<SerialPacket*>(packetBuffer));
		}
		//update the internals
		//TODO: update the internals i.e. player positions
		//give the computer a break
		SDL_Delay(10);
	}
}

void ServerApplication::Quit() {
	std::cout << "Shutting down" << std::endl;

	//APIs
	lua_close(luaState);
	sqlite3_close_v2(database);
	network.Close();
	SDLNet_Quit();
	SDL_Quit();

	std::cout << "Shutdown finished" << std::endl;
}

//-------------------------
//handle incoming traffic
//-------------------------

void ServerApplication::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		//basic connections
		case SerialPacketType::BROADCAST_REQUEST:
			HandleBroadcastRequest(dynamic_cast<SerialPacket*>(argPacket));
		break;
		case SerialPacketType::JOIN_REQUEST:
			HandleJoinRequest(dynamic_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT:
			HandleDisconnect(dynamic_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::SHUTDOWN:
			HandleShutdown(dynamic_cast<SerialPacket*>(argPacket));
		break;

		//map management
		case SerialPacketType::REGION_REQUEST:
			HandleRegionRequest(dynamic_cast<RegionPacket*>(argPacket));
		break;

		//combat management
		//TODO: combat management

		//character management
		case SerialPacketType::CHARACTER_NEW:
			HandleCharacterNew(dynamic_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			HandleCharacterDelete(dynamic_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST: //TODO: ?
			HandleCharacterUpdate(dynamic_cast<CharacterPacket*>(argPacket));
		break;

		//enemy management
		//TODO: enemy management

		//mismanagement
		case SerialPacketType::SYNCHRONIZE:
			HandleSynchronize(dynamic_cast<ClientPacket*>(argPacket));
		break;

		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacketType encountered in the server: " + to_string_custom(int(argPacket->type))));
		break;
	}
}

//-------------------------
//basic connections
//-------------------------

void ServerApplication::HandleBroadcastRequest(SerialPacket* const argPacket) {
	//send the server's data
	ServerPacket newPacket;

	newPacket.type = SerialPacketType::BROADCAST_RESPONSE;
	snprintf(newPacket.name, PACKET_STRING_SIZE, "%s", config["server.name"].c_str());
	newPacket.playerCount = characterMgr.GetContainer()->size();
	newPacket.version = NETWORK_VERSION;

	network.SendTo(&argPacket->srcAddress, dynamic_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::HandleJoinRequest(ClientPacket* const argPacket) {
	//create the new client
	ClientData newClient;
	newClient.address = argPacket->srcAddress;

	//load the user account
	//TODO: handle passwords
	int accountIndex = accountMgr.LoadAccount(argPacket->username, clientUID);
	if (accountIndex < 0) {
		//TODO: send rejection packet
		std::cerr << "Error: Account already loaded: " << accountIndex << std::endl;
		return;
	}

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::JOIN_RESPONSE;
	newPacket.clientIndex = clientUID;
	newPacket.accountIndex = accountIndex;

	network.SendTo(&newClient.address, dynamic_cast<SerialPacket*>(&newPacket));

	//finished this routine
	clientMap[clientUID++] = newClient;
	std::cout << "New connection, " << clientMap.size() << " clients and " << accountMgr.GetContainer()->size() << " accounts total" << std::endl;
}

void ServerApplication::HandleDisconnect(ClientPacket* const argPacket) {
	//TODO: authenticate who is disconnecting/kicking

	//forward to the specified client
	network.SendTo(
		&clientMap[ accountMgr.GetAccount(argPacket->accountIndex)->clientIndex ].address,
		dynamic_cast<SerialPacket*>(argPacket)
	);

	//save and unload this account's characters
	//pump the unload message to all remaining clients
	characterMgr.UnloadCharacterIf([&](std::map<int, CharacterData>::iterator it) -> bool {
		if (argPacket->accountIndex == it->second.owner) {
			PumpCharacterUnload(it->first);
			return true;
		}
		return false;
	});

	//erase the in-memory stuff
	clientMap.erase(accountMgr.GetAccount(argPacket->accountIndex)->clientIndex);
	accountMgr.UnloadAccount(argPacket->accountIndex);

	//finished this routine
	std::cout << "Disconnection, " << clientMap.size() << " clients and " << accountMgr.GetContainer()->size() << " accounts total" << std::endl;
}

void ServerApplication::HandleShutdown(SerialPacket* const argPacket) {
	//TODO: authenticate who is shutting the server down

	//end the server
	running = false;

	//disconnect all clients
	SerialPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT;
	PumpPacket(&newPacket);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

//-------------------------
//map management
//-------------------------

void ServerApplication::HandleRegionRequest(RegionPacket* const argPacket) {
	RegionPacket newPacket;

	newPacket.type = SerialPacketType::REGION_CONTENT;
	newPacket.roomIndex = argPacket->roomIndex;
	newPacket.x = argPacket->x;
	newPacket.y = argPacket->y;

	newPacket.region = roomMgr.GetRoom(argPacket->roomIndex)->pager.GetRegion(argPacket->x, argPacket->y);

	//send the content
	network.SendTo(&argPacket->srcAddress, dynamic_cast<SerialPacket*>(argPacket));
}

//-------------------------
//combat management
//-------------------------

//TODO: combat management

//-------------------------
//Character Management
//-------------------------

void ServerApplication::HandleCharacterNew(CharacterPacket* const argPacket) {
	//TODO: fill this
}

void ServerApplication::HandleCharacterDelete(CharacterPacket* const argPacket) {
	//TODO: fill this
}

void ServerApplication::HandleCharacterUpdate(CharacterPacket* const argPacket) {
	CharacterData* character = characterMgr.GetCharacter(argPacket->characterIndex);

	//make a new character if this one doesn't exist
	if (!character) {
		//this isn't normal
		std::cerr << "Warning: HandleCharacterUpdate() is passing to HandleCharacterNew()" << std::endl;
		HandleCharacterNew(argPacket);
		return;
	}

	/* TODO: rewrite this design flaw, read more
	 * Slaving the client to the server here is a terrible idea, instead there
	 * needs to be a utility function to update and send the server-side character
	 * to the clients.
	 *
	 * Other things to consider include functionality to reequip the character,
	 * apply status effects and to change the stats as well. These should all be
	 * handled server-side.
	*/
	character->roomIndex = argPacket->roomIndex;
	character->origin = argPacket->origin;
	character->motion = argPacket->motion;

	character->stats = argPacket->stats;

	//TODO: equipment
	//TODO: items
	//TODO: buffs
	//TODO: debuffs

	PumpPacket(argPacket);
}

//-------------------------
//enemy management
//-------------------------

//TODO: enemy management

//-------------------------
//mismanagement
//-------------------------

void ServerApplication::HandleSynchronize(ClientPacket* const argPacket) {
	//TODO: compensate for large distances
	//TODO: I quite dislike this function

	//send all the server's data to this client
	CharacterPacket characterPacket;

	//characters
	characterPacket.type = SerialPacketType::CHARACTER_UPDATE;
	for (auto& it : *characterMgr.GetContainer()) {
		//TODO: update this for the expanded CharacterData structure
		characterPacket.characterIndex = it.first;
		snprintf(characterPacket.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(characterPacket.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		characterPacket.roomIndex = it.second.roomIndex;
		characterPacket.origin = it.second.origin;
		characterPacket.motion = it.second.motion;
		characterPacket.stats = it.second.stats;

		network.SendTo(&clientMap[argPacket->clientIndex].address, dynamic_cast<SerialPacket*>(&characterPacket));
	}

	//TODO: more
}

//-------------------------
//utility methods
//-------------------------

//TODO: a function that only sends to characters in a certain proximity

void ServerApplication::PumpPacket(SerialPacket* const argPacket) {
	for (auto& it : clientMap) {
		network.SendTo(&it.second.address, argPacket);
	}
}

void ServerApplication::PumpCharacterUnload(int uid) {
	//delete the client-side character(s)
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = uid;
	PumpPacket(dynamic_cast<SerialPacket*>(&newPacket));
}
