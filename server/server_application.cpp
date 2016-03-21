/* Copyright: (c) Kayne Ruse 2013-2015
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

//utility functions
#include "sql_tools.hpp"

//std & STL
#include <chrono>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>

//-------------------------
//public methods
//-------------------------

//BUG: #35 The server fails without at least one room

void ServerApplication::Init(int argc, char* argv[]) {
	//NOTE: I might need to rearrange the init process so that lua & SQL can interact with the map system as needed.
	std::cout << "Beginning " << argv[0] << std::endl;

	//load the config settings
	config.Load("rsc/config.cfg", false, argc, argv);

	//-------------------------
	//Initialize the APIs
	//-------------------------

	//Init SDL
	if (SDL_Init(0)) {
		std::ostringstream os;
		os << "Failed to initialize SDL: " <<  SDL_GetError();
		throw(std::runtime_error(os.str()));
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

	//append config["dir.scripts"] to the module path
	if (config["dir.scripts"].size() > 0) {
		//get the original path
		lua_getglobal(luaState, "package");
		lua_getfield(luaState, -1, "path");

		//build & push the message
		std::ostringstream path;
		path << lua_tostring(luaState, -1) << ";" << config["dir.scripts"] << "?.lua";
		lua_pushstring(luaState, path.str().c_str());

		//set the new path and clean up the stack
		lua_setfield(luaState, -3, "path");
		lua_pop(luaState, 2);

		std::cout << "\tLua script directory appended" << std::endl;
	}

	//-------------------------
	//Setup the objects
	//-------------------------

	//set the hooks
	accountMgr.SetDatabase(database);
	characterMgr.SetDatabase(database);

	roomMgr.SetLuaState(luaState);
	roomMgr.SetDatabase(database);

	std::cout << "Internal managers initialized" << std::endl;

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

#define DEBUG_INTERNAL_VAR(x) std::cout << "\t" << #x << ": " << x << std::endl;

std::cout << "Internal sizes:" << std::endl;

	DEBUG_INTERNAL_VAR(config["server.name"]);
	DEBUG_INTERNAL_VAR(NETWORK_VERSION);
	DEBUG_INTERNAL_VAR(sizeof(Region::type_t));
	DEBUG_INTERNAL_VAR(sizeof(Region));
	DEBUG_INTERNAL_VAR(REGION_WIDTH);
	DEBUG_INTERNAL_VAR(REGION_HEIGHT);
	DEBUG_INTERNAL_VAR(REGION_DEPTH);
	DEBUG_INTERNAL_VAR(REGION_TILE_FOOTPRINT);
	DEBUG_INTERNAL_VAR(REGION_SOLID_FOOTPRINT);
	DEBUG_INTERNAL_VAR(PACKET_STRING_SIZE);
	DEBUG_INTERNAL_VAR(PACKET_BUFFER_SIZE);
	DEBUG_INTERNAL_VAR(MAX_PACKET_SIZE);
	DEBUG_INTERNAL_VAR(static_cast<int>(SerialPacketType::LAST));

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
	//network buffer
	SerialPacket* packetBuffer = reinterpret_cast<SerialPacket*>(new char[MAX_PACKET_SIZE]);
	memset(packetBuffer, 0, MAX_PACKET_SIZE); //zero the buffer

	//time system
	typedef std::chrono::steady_clock Clock;

	Clock::time_point simTime = Clock::now();
	Clock::time_point realTime;

	while(running) {
		//suck in the waiting packets & process them
		while(network.Receive(packetBuffer)) {
			try {
				HandlePacket(packetBuffer);
			}
			catch(std::exception& e) {
				std::cerr << "HandlePacket Error: " << e.what() << std::endl;
			}
			//reset the buffer
			memset(packetBuffer, 0, MAX_PACKET_SIZE);
		}

		//Check client connections
		std::list<int> disconnections = clientMgr.CheckConnections();
		for(auto const& it : disconnections) {
			fullClientUnload(it);
			std::cerr << "Client dropped: " << it << std::endl;
		}

		//"tick" the internals
		realTime = Clock::now();

		if (simTime < realTime) {
			while(simTime < realTime) {
				for (auto& it : *roomMgr.GetContainer()) {
					it.second.RunFrame();
				}
				//~60 FPS
				simTime += std::chrono::duration<int, std::milli>(16);
			}
		}
		else {
			//give the machine a break
			SDL_Delay(10);
		}
	}
	delete reinterpret_cast<char*>(packetBuffer);
}

void ServerApplication::Quit() {
	std::cout << "Shutting down" << std::endl;

	//save the server state
	SaveServerState();

	//close the managers
	accountMgr.UnloadAll();
	characterMgr.UnloadAll();
	clientMgr.UnloadAll();
	roomMgr.UnloadAll();

	//APIs
	lua_close(luaState);
	sqlite3_close_v2(database);
	network.Close();
	SDLNet_Quit();
	SDL_Quit();

	std::cout << "Clean exit" << std::endl;
}

//-------------------------
//handle incoming traffic
//-------------------------

void ServerApplication::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		//heartbeat system
		case SerialPacketType::PING:
			hPing(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::PONG:
			hPong(static_cast<ServerPacket*>(argPacket));
		break;

		//client connections
		case SerialPacketType::BROADCAST_REQUEST:
			hBroadcastRequest(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::JOIN_REQUEST:
			hJoinRequest(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::LOGIN_REQUEST:
			hLoginRequest(static_cast<ClientPacket*>(argPacket));
		break;

		//client disconnections
		case SerialPacketType::LOGOUT_REQUEST:
			hLogoutRequest(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT_REQUEST:
			hDisconnectRequest(static_cast<ClientPacket*>(argPacket));
		break;

		//server commands
		case SerialPacketType::ADMIN_DISCONNECT_FORCED:
			hAdminDisconnectForced(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::ADMIN_SHUTDOWN_REQUEST:
			hAdminShutdownRequest(static_cast<ClientPacket*>(argPacket));
		break;

		//data management & queries
		case SerialPacketType::REGION_REQUEST:
			hRegionRequest(static_cast<RegionPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_EXISTS:
			hQueryCharacterExists(static_cast<CharacterPacket*>(argPacket));
		break;

		//character management
		case SerialPacketType::CHARACTER_CREATE:
			hCharacterCreate(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			hCharacterDelete(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_LOAD:
			hCharacterLoad(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_UNLOAD:
			hCharacterUnload(static_cast<CharacterPacket*>(argPacket));
		break;

		//character movement
		case SerialPacketType::CHARACTER_MOVEMENT:
			hCharacterMovement(static_cast<CharacterPacket*>(argPacket));
		break;

		case SerialPacketType::QUERY_CREATURE_EXISTS:
			//TODO: creature queries
		break;

		//chat
		case SerialPacketType::TEXT_BROADCAST:
			hTextBroadcast(static_cast<TextPacket*>(argPacket));
		break;
		case SerialPacketType::TEXT_SPEECH:
			hTextSpeech(static_cast<TextPacket*>(argPacket));
		break;
		case SerialPacketType::TEXT_WHISPER:
			hTextWhisper(static_cast<TextPacket*>(argPacket));
		break;

		//handle errors
		default: {
			std::ostringstream msg;
			msg << "Unknown SerialPacketType encountered in the server: ";
			msg << static_cast<int>(argPacket->type);
			throw(std::runtime_error(msg.str()));
		}
		break;
	}
}

//-------------------------
//heartbeat system
//-------------------------

void ServerApplication::hPing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void ServerApplication::hPong(ServerPacket* const argPacket) {
	clientMgr.HandlePong(argPacket);
}

//-------------------------
//basic connections
//-------------------------

void ServerApplication::hBroadcastRequest(ServerPacket* const argPacket) {
	//send the server's data
	ServerPacket newPacket;

	newPacket.type = SerialPacketType::BROADCAST_RESPONSE;
	strncpy(newPacket.name, config["server.name"].c_str(), PACKET_STRING_SIZE);
	newPacket.playerCount = characterMgr.GetLoadedCount();
	newPacket.version = NETWORK_VERSION;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::hJoinRequest(ClientPacket* const argPacket) {
	//register the client
	int clientIndex = clientMgr.Create(argPacket->srcAddress);

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::JOIN_RESPONSE;
	newPacket.clientIndex = clientIndex;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

	//finished this routine
	std::cout << "New join, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::hLoginRequest(ClientPacket* const argPacket) {
	//get the client data
	ClientData* clientData = clientMgr.Get(argPacket->clientIndex);

	if (clientData == nullptr || clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified client index detected: " << argPacket->clientIndex << std::endl;
		return;
	}

	//load the user account
	int accountIndex = accountMgr.Load(argPacket->username, argPacket->clientIndex);

	//Cannot load
	if (accountIndex < 0) {
		//build the message
		std::ostringstream msg;
		msg << "Account already loaded: " << argPacket->username;

		//build the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::LOGIN_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

		//log the error
		std::cerr << "Error message sent: " << newPacket.text << std::endl;
		return;
	}

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::LOGIN_RESPONSE;
	newPacket.clientIndex = argPacket->clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

	//finished this routine
	std::cout << "New login, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

//-------------------------
//client disconnections
//-------------------------

void ServerApplication::hLogoutRequest(ClientPacket* const argPacket) {
	//get the account and client data
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	if (!accountData) {
		return;
	}

	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		std::ostringstream msg;
		msg << "No client found for an account: " << accountData->GetUsername();
		throw(std::logic_error(msg.str()));
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified logout detected targeting: " << accountData->GetUsername() << std::endl;
		return;
	}

	//send the logout response
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::LOGOUT_RESPONSE;
	newPacket.accountIndex = argPacket->accountIndex;

	network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

	//save and unload this account and it's characters
	fullAccountUnload(argPacket->accountIndex);

	//finished this routine
	std::cout << "New logout, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::hDisconnectRequest(ClientPacket* const argPacket) {
	//get the client data
	ClientData* clientData = clientMgr.Get(argPacket->clientIndex);
	if (!clientData) {
		return;
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified disconnection detected targeting: " << argPacket->clientIndex << std::endl;
		return;
	}

	//send the disconnect response
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT_RESPONSE;
	newPacket.clientIndex = argPacket->clientIndex;

	network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

	//unload the client, it's accounts, and their characters
	fullClientUnload(argPacket->clientIndex);

	//finished this routine
	std::cout << "New disconnection, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;	
}

//-------------------------
//server commands
//-------------------------

void ServerApplication::hAdminDisconnectForced(ClientPacket* const argPacket) {
	//TODO: (9) ServerApplication::hAdminDisconnectForced()
}

void ServerApplication::hAdminShutdownRequest(ClientPacket* const argPacket) {
	//get the account and client data
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	if (!accountData) {
		return;
	}
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		std::ostringstream msg;
		msg << "No client found for an account: " << accountData->GetUsername();
		throw(std::logic_error(msg.str()));
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified server shutdown detected from: " << accountData->GetUsername() << std::endl;
		return;
	}

	//reject non-admin requests
	if (accountData->GetAdministrator() != true) {
		std::cerr << "Rejected server shutdown command from: " << accountData->GetUsername() << std::endl;

		//build the message
		std::ostringstream msg;
		msg << "Invalid admin status";

		//build the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::SHUTDOWN_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);

		//send the rejection message
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//end the server
	running = false;

	//disconnect all clients
	TextPacket newPacket;
	newPacket.type = SerialPacketType::ADMIN_DISCONNECT_FORCED;
	strncpy(newPacket.text, "Server shutdown", PACKET_STRING_SIZE);
	pumpPacket(&newPacket);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

//-------------------------
//Queries
//-------------------------

void ServerApplication::hRegionRequest(RegionPacket* const argPacket) {
	//get the region object, send a rejection on error
	RoomData* room = roomMgr.Get(argPacket->roomIndex);
	if (!room) {
		//build the error message
		std::ostringstream msg;
		msg << "Failed to find Region (" << argPacket->roomIndex << "," << argPacket->x << "," << argPacket->y << "); ";
		msg << "Room " << argPacket->roomIndex << " does not exist";

		//build the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::REGION_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		//log the error
		std::cerr << "Error message sent: " << newPacket.text << std::endl;
		return;
	}

	//A strange reoccurance of #45 caused this
	Region* region = room->GetPager()->FindRegion(argPacket->x, argPacket->y);
	//TODO: (0) load regions
	if (region == nullptr) {
		region = room->GetPager()->CreateRegion(argPacket->x, argPacket->y);
		std::cout << "Summoned a room" << std::endl;
	}

	//send the content
	RegionPacket newPacket;

	newPacket.type = SerialPacketType::REGION_CONTENT;
	newPacket.roomIndex = argPacket->roomIndex;
	newPacket.x = argPacket->x;
	newPacket.y = argPacket->y;
	newPacket.region = region;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::hQueryCharacterExists(CharacterPacket* const argPacket) {
	//respond with all character data
	CharacterPacket newPacket;

	for (auto& it : *characterMgr.GetContainer()) {
		if (argPacket->roomIndex != -1 && it.second.GetRoomIndex() != argPacket->roomIndex) {
			continue;
		}
		copyCharacterToPacket(&newPacket, it.first);
		newPacket.type = SerialPacketType::QUERY_CHARACTER_EXISTS;
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
	}
}

//-------------------------
//Character Management
//-------------------------

void ServerApplication::hCharacterCreate(CharacterPacket* const argPacket) {
	int characterIndex = characterMgr.Create(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		msg << "Character already exists: " << argPacket->handle;

		//build & send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//push to the rooms
	CharacterData* characterData = characterMgr.Get(characterIndex);
	roomMgr.PushCharacter(characterData);

	//pump this character to all clients
	CharacterPacket newPacket;
	copyCharacterToPacket(&newPacket, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
}

void ServerApplication::hCharacterDelete(CharacterPacket* const argPacket) {
	//get the user's data
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	if (!accountData) {
		return;
	}
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		return;
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified character deletion detected targeting: " << argPacket->handle << std::endl;
		return;
	}

	//load the character into memory
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		msg << "Cannot delete this character";

		//build & send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//pop from the rooms
	CharacterData* characterData = characterMgr.Get(characterIndex);
	roomMgr.PopCharacter(characterData);

	//pump character unload
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_UNLOAD;
	newPacket.characterIndex = characterIndex;
	pumpPacketProximity(static_cast<SerialPacket*>(&newPacket), characterData->GetRoomIndex());

	//delete the character from the database
	characterMgr.Delete(characterIndex);
}

void ServerApplication::hCharacterLoad(CharacterPacket* const argPacket) {
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		if (characterIndex == -1) {
			msg << "Character already loaded: ";
		}
		if (characterIndex == -2) {
			msg << "Character name is taken: ";
		}
		msg << argPacket->handle;

		//build & send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//push to the rooms
	CharacterData* characterData = characterMgr.Get(characterIndex);
	roomMgr.PushCharacter(characterData);

	//pump this character to all clients
	CharacterPacket newPacket;
	copyCharacterToPacket(&newPacket, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
}

void ServerApplication::hCharacterUnload(CharacterPacket* const argPacket) {
	//get the entries
	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);
	if (!characterData) {
		return;
	}

	AccountData* accountData = accountMgr.Get(characterData->GetOwner());
	if (!accountData) {
		return;
	}

	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		return;
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified character unload detected targeting: uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//pop from the rooms
	roomMgr.PopCharacter(characterData);

	//pump character delete
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_UNLOAD;
	newPacket.characterIndex = argPacket->characterIndex;
	pumpPacketProximity(static_cast<SerialPacket*>(&newPacket), characterData->GetRoomIndex());

	//unload the character
	characterMgr.Unload(argPacket->characterIndex);
}

//-------------------------
//character movement
//-------------------------

//TODO: (2) Could replace this verbosity with a "verify" method, taking a client, account and character ptr as arguments

void ServerApplication::hCharacterMovement(CharacterPacket* const argPacket) {
	//get the specified objects
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);

	if (!accountData) {
		throw(std::runtime_error("Failed to move a character, missing account"));
	}

	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);

	if (!characterData) {
		throw(std::runtime_error("Failed to move a character, missing character"));
	}

	//get this account's client
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified set character motion targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//check if allowed
	if (characterData->GetOwner() != argPacket->accountIndex && !accountData->GetModerator() && !accountData->GetAdministrator()) {
		//TODO: (3) send to the client?
		std::cerr << "Failed to set character motion due to lack of permissions targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//check if moving rooms
	if (characterData->GetRoomIndex() != argPacket->roomIndex) {
		//set the character's origin and motion
		characterData->SetOrigin(argPacket->origin);
		characterData->SetMotion(argPacket->motion);

		//send the delete & create messages
		pumpAndChangeRooms(characterData, argPacket->roomIndex, argPacket->characterIndex);
	}
	//if not moving between rooms
	else {
		//set the character's origin and motion
		characterData->SetOrigin(argPacket->origin);
		characterData->SetMotion(argPacket->motion);

		//update the clients
		CharacterPacket newPacket;
		copyCharacterToPacket(&newPacket, argPacket->characterIndex);
		newPacket.type = SerialPacketType::CHARACTER_MOVEMENT;
		pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
	}
}

//-------------------------
//chat
//-------------------------

void ServerApplication::hTextBroadcast(TextPacket* const argPacket) {
	//TODO: (9) ServerApplication::hTextBroadcast()
}

void ServerApplication::hTextSpeech(TextPacket* const argPacket) {
	//TODO: (9) ServerApplication::hTextSpeech()
}

void ServerApplication::hTextWhisper(TextPacket* const argPacket) {
	//TODO: (9) ServerApplication::hTextWhisper()
}

//-------------------------
//utility methods
//-------------------------

void ServerApplication::SaveServerState() {
	//TODO: (3) Periodic mass server saves
}

