/* Copyright: (c) Kayne Ruse 2013, 2014
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
#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

//server specific stuff
#include "account_manager.hpp"
#include "character_manager.hpp"
#include "client_data.hpp"
#include "combat_manager.hpp"
#include "enemy_manager.hpp"
#include "room_manager.hpp"

//maps
#include "map_allocator.hpp"
#include "map_file_format.hpp"
#include "region_pager.hpp"

//common utilities
#include "udp_network_utility.hpp"
#include "config_utility.hpp"

//APIs
#include "lua/lua.hpp"
#include "sqlite3/sqlite3.h"
#include "SDL/SDL.h"

//STL
#include <map>
#include <string>

//The main application class
class ServerApplication {
public:
	//public methods
	ServerApplication() = default;
	~ServerApplication() = default;

	void Init(int argc, char** argv);
	void Proc();
	void Quit();

private:
	//handle incoming traffic
	void HandlePacket(SerialPacket);

	//basic connections
	void HandleBroadcastRequest(SerialPacket);
	void HandleJoinRequest(SerialPacket);
	void HandleDisconnect(SerialPacket);
	void HandleShutdown(SerialPacket);

	//map management
	void HandleRegionRequest(SerialPacket);

	//combat management
	//TODO: combat management

	//character management
	void HandleCharacterNew(SerialPacket);
	void HandleCharacterDelete(SerialPacket);
	void HandleCharacterUpdate(SerialPacket);

	//enemy management
	//TODO: enemy management

	//mismanagement
	void HandleSynchronize(SerialPacket);

	//utility methods
	//TODO: a function that only sends to characters in a certain proximity
	void PumpPacket(SerialPacket);
	void PumpCharacterUnload(int uid);

	//APIs and utilities
	sqlite3* database = nullptr;
	lua_State* luaState = nullptr;
	UDPNetworkUtility network;
	ConfigUtility config;

	//simple tables
	std::map<int, ClientData> clientMap;

	//managers
	AccountManager accountMgr;
	CharacterManager characterMgr;
	CombatManager combatMgr;
	EnemyManager enemyMgr;
	RoomManager roomMgr;

	//misc
	bool running = true;
	int clientUID = 0;
};

#endif
