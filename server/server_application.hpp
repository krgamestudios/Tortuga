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

//server specific stuff, mostly managers
#include "client_data.hpp"
#include "account_manager.hpp"
#include "character_manager.hpp"
#include "room_manager.hpp"

//common utilities
#include "udp_network_utility.hpp"
#include "serial_packet.hpp"
#include "config_utility.hpp"
#include "singleton.hpp"

//APIs
#include "lua/lua.hpp"
#include "sqlite3/sqlite3.h"
#include "SDL/SDL.h"

//STL
#include <map>
#include <string>

//The main application class
class ServerApplication: public Singleton<ServerApplication> {
public:
	//public methods
	void Init(int argc, char** argv);
	void Proc();
	void Quit();

private:
	friend Singleton<ServerApplication>;

	ServerApplication() = default;
	~ServerApplication() = default;

	//handle incoming traffic
	void HandlePacket(SerialPacket* const);

	//basic connections
	void HandleBroadcastRequest(ServerPacket* const);
	void HandleJoinRequest(ClientPacket* const);
	void HandleDisconnect(ClientPacket* const);
	void HandleShutdown(ClientPacket* const);

	//map management
	void HandleRegionRequest(RegionPacket* const);

	//character management
	void HandleCharacterNew(CharacterPacket* const);
	void HandleCharacterDelete(CharacterPacket* const);
	void HandleCharacterUpdate(CharacterPacket* const);

	//mismanagement
	void HandleSynchronize(ClientPacket* const);

	//utility methods
	//TODO: a function that only sends to characters in a certain proximity
	void PumpPacket(SerialPacket* const);
	void PumpCharacterUnload(int uid);
	void CopyCharacterToPacket(CharacterPacket* const packet, int characterIndex);

	//APIs and utilities
	sqlite3* database = nullptr;
	lua_State* luaState = nullptr;
	UDPNetworkUtility& network = UDPNetworkUtility::GetSingleton();
	ConfigUtility& config = ConfigUtility::GetSingleton();

	//simple tables
	std::map<int, ClientData> clientMap;

	//managers
	AccountManager& accountMgr = AccountManager::GetSingleton();
	CharacterManager& characterMgr = CharacterManager::GetSingleton();
	RoomManager& roomMgr = RoomManager::GetSingleton();

	//misc
	bool running = true;
	int clientIndex = 0;
};

#endif
