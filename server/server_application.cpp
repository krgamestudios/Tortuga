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
#include "utility.hpp"

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

	//set the hooks
	accountMgr.SetDatabase(database);
	characterMgr.SetDatabase(database);

	combatMgr.SetLuaState(luaState);
	roomMgr.SetLuaState(luaState);
	enemyMgr.SetLuaState(luaState);

	std::cout << "Internal managers set" << std::endl;

	//register the "globals"
	lua_pushstring(luaState, ROOM_MANAGER_PSEUDOINDEX);
	lua_pushlightuserdata(luaState, &roomMgr);
	lua_settable(luaState, LUA_REGISTRYINDEX);

	std::cout << "Internal managers registered with lua" << std::endl;

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
	std::cout << "\tPACKET_BUFFER_SIZE: " << PACKET_BUFFER_SIZE << std::endl;
	std::cout << "\tMAX_PACKET_SIZE: " << MAX_PACKET_SIZE << std::endl;

	//-------------------------
	//finalize the startup
	//-------------------------

	std::cout << "Startup completed successfully" << std::endl;

	//-------------------------
	//debugging
	//-------------------------

	//...
}

void ServerApplication::Proc() {
	SerialPacket* packetBuffer = static_cast<SerialPacket*>(malloc(MAX_PACKET_SIZE));
	while(running) {
		//suck in the waiting packets & process them
		while(network.Receive(packetBuffer)) {
			HandlePacket(packetBuffer);
		}
		//update the internals
		//TODO: update the internals i.e. player positions
		//give the computer a break
		SDL_Delay(10);
	}
	free(static_cast<void*>(packetBuffer));
}

void ServerApplication::Quit() {
	std::cout << "Shutting down" << std::endl;

	//close the managers
	clientMap.clear();
	accountMgr.UnloadAll();
	characterMgr.UnloadAll();
	//TODO: unload combats
	//TODO: unload enemies
	//TODO: unload rooms

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
			HandleBroadcastRequest(static_cast<SerialPacket*>(argPacket));
		break;
		case SerialPacketType::JOIN_REQUEST:
			HandleJoinRequest(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT:
			HandleDisconnect(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::SHUTDOWN:
			HandleShutdown(static_cast<SerialPacket*>(argPacket));
		break;

		//map management
		case SerialPacketType::REGION_REQUEST:
			HandleRegionRequest(static_cast<RegionPacket*>(argPacket));
		break;

		//combat management
		//TODO: combat management

		//character management
		case SerialPacketType::CHARACTER_NEW:
			HandleCharacterNew(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			HandleCharacterDelete(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_UPDATE:
		case SerialPacketType::CHARACTER_STATS_REQUEST:
			HandleCharacterUpdate(static_cast<CharacterPacket*>(argPacket));
		break;

		//enemy management
		//TODO: enemy management

		//mismanagement
		case SerialPacketType::SYNCHRONIZE:
			HandleSynchronize(static_cast<ClientPacket*>(argPacket));
		break;

		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacketType encountered in the server: " + to_string_custom(static_cast<int>(argPacket->type)) ));
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
	strncpy(newPacket.name, config["server.name"].c_str(), PACKET_STRING_SIZE);
	newPacket.playerCount = characterMgr.GetContainer()->size();
	newPacket.version = NETWORK_VERSION;

	network.SendTo(&argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
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

	network.SendTo(&newClient.address, static_cast<SerialPacket*>(&newPacket));

	//finished this routine
	clientMap[clientUID++] = newClient;
	std::cout << "New connection, " << clientMap.size() << " clients and " << accountMgr.GetContainer()->size() << " accounts total" << std::endl;
}

void ServerApplication::HandleDisconnect(ClientPacket* const argPacket) {
	//TODO: authenticate who is disconnecting/kicking

	//forward to the specified client
	network.SendTo(
		&clientMap[ accountMgr.GetAccount(argPacket->accountIndex)->clientIndex ].address,
		static_cast<SerialPacket*>(argPacket)
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
	network.SendTo(&argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

//-------------------------
//combat management
//-------------------------

//TODO: combat management

//-------------------------
//Character Management
//-------------------------

void ServerApplication::HandleCharacterNew(CharacterPacket* const argPacket) {
	//BUG: #27 Characters can be created with an invalid account index
	//NOTE: misnomer, try to load the character first
	int characterIndex = characterMgr.LoadCharacter(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex == -1) {
		//TODO: rejection packet
		std::cerr << "Warning: Character already loaded" << std::endl;
		return;
	}

	if (characterIndex == -2) {
		//TODO: rejection packet
		std::cerr << "Warning: Character already exists" << std::endl;
		return;
	}

	//send this new character to all clients
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_NEW;
	CopyCharacterToPacket(&newPacket, characterIndex);
	PumpPacket(&newPacket);
}

void ServerApplication::HandleCharacterDelete(CharacterPacket* const argPacket) {
	//NOTE: Disconnecting only unloads a character, this explicitly deletes it

	//Authenticate the owner is doing this
	int characterIndex = characterMgr.LoadCharacter(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	//if this is not your character
	if (characterIndex == -2) {
		//TODO: rejection packet
		std::cerr << "Warning: Character cannot be deleted" << std::endl;

		//unload an unneeded character
		if (characterIndex != -1) {
			characterMgr.UnloadCharacter(characterIndex);
		}
		return;
	}

	//delete it
	characterMgr.DeleteCharacter(characterIndex);

	//TODO: success packet

	//Unload this character from all clients
	PumpCharacterUnload(characterIndex);
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

	//TODO: gameplay components: equipment, items, buffs, debuffs

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
	//NOTE: I quite dislike this function

	//send all of the server's data to this client
	ClientData& client = clientMap[argPacket->clientIndex];

	//send all characters
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_UPDATE;

	for (auto& it : *characterMgr.GetContainer()) {
		newPacket.characterIndex = it.first;
		CopyCharacterToPacket(&newPacket, it.first);
		network.SendTo(&client.address, static_cast<SerialPacket*>(&newPacket));
	}

	//TODO: more in HandleSynchronize()
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
	PumpPacket(static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::CopyCharacterToPacket(CharacterPacket* const packet, int characterIndex) {
	CharacterData* character = characterMgr.GetCharacter(characterIndex);
	if (!character) {
		throw(std::runtime_error("Failed to copy a character to a packet"));
	}

	//TODO: keep this up to date when the character changes
	packet->characterIndex = characterIndex;
	strncpy(packet->handle, character->handle.c_str(), PACKET_STRING_SIZE);
	strncpy(packet->avatar, character->avatar.c_str(), PACKET_STRING_SIZE);
	packet->accountIndex = character->owner;
	packet->roomIndex = character->roomIndex;
	packet->origin = character->origin;
	packet->motion = character->motion;
	packet->stats = character->stats;
}