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
#include <stdexcept>
#include <chrono>
 #include <cstring>
#include <iostream>
#include <list>
#include <sstream>
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

#define DEBUG_OUTPUT_VAR(x) std::cout << "\t" << #x << ": " << x << std::endl;

	std::cout << "Internal sizes:" << std::endl;

	DEBUG_OUTPUT_VAR(NETWORK_VERSION);
	DEBUG_OUTPUT_VAR(sizeof(Region::type_t));
	DEBUG_OUTPUT_VAR(sizeof(Region));
	DEBUG_OUTPUT_VAR(REGION_WIDTH);
	DEBUG_OUTPUT_VAR(REGION_HEIGHT);
	DEBUG_OUTPUT_VAR(REGION_DEPTH);
	DEBUG_OUTPUT_VAR(REGION_TILE_FOOTPRINT);
	DEBUG_OUTPUT_VAR(REGION_SOLID_FOOTPRINT);
	DEBUG_OUTPUT_VAR(PACKET_STRING_SIZE);
	DEBUG_OUTPUT_VAR(PACKET_BUFFER_SIZE);
	DEBUG_OUTPUT_VAR(MAX_PACKET_SIZE);
	DEBUG_OUTPUT_VAR(static_cast<int>(SerialPacketType::LAST));

#undef DEBUG_OUTPUT_VAR

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
		case SerialPacketType::QUERY_CHARACTER_STATS:
			hQueryCharacterStats(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_LOCATION:
			hQueryCharacterLocation(static_cast<CharacterPacket*>(argPacket));
		break;

		case SerialPacketType::QUERY_MONSTER_EXISTS:
			hQueryMonsterExists(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_MONSTER_STATS:
			hQueryMonsterStats(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_MONSTER_LOCATION:
			hQueryMonsterLocation(static_cast<MonsterPacket*>(argPacket));
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
		case SerialPacketType::CHARACTER_ATTACK:
			hCharacterAttack(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DAMAGE:
			hCharacterDamage(static_cast<CharacterPacket*>(argPacket));
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
