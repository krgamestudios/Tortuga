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

#include <stdexcept>
#include <iostream>
#include <string>

//-------------------------
//Define the various UIDs
//-------------------------

int ClientData::uidCounter = 0;

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
	network.Open(config.Int("server.port"), PACKET_BUFFER_SIZE);
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
	//TODO: config parameter
	regionPager.GetFormat()->SetSaveDir("save/mapname/");
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
		while(network.Receive()) {
			//get the packet
			deserialize(&packet, network.GetInData());
			//cache the source address
			packet.meta.srcAddress = network.GetInPacket()->address;
			//we need to go deeper
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
//Define the uber switch
//-------------------------

void ServerApplication::HandlePacket(SerialPacket packet) {
	switch(packet.meta.type) {
		case SerialPacket::Type::BROADCAST_REQUEST:
			HandleBroadcastRequest(packet);
		break;
		case SerialPacket::Type::JOIN_REQUEST:
			HandleJoinRequest(packet);
		break;
		case SerialPacket::Type::SYNCHRONIZE:
			HandleSynchronize(packet);
		break;
		case SerialPacket::Type::DISCONNECT:
			HandleDisconnect(packet);
		break;
		case SerialPacket::Type::SHUTDOWN:
			HandleShutdown(packet);
		break;
		case SerialPacket::Type::CHARACTER_UPDATE:
			HandleCharacterUpdate(packet);
		break;
		case SerialPacket::Type::REGION_REQUEST:
			HandleRegionRequest(packet);
		break;
		//handle errors
		default:
			throw(std::runtime_error("Unknown SerialPacket::Type encountered"));
		break;
	}
}
