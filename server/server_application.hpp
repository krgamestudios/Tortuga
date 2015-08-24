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
#pragma once

//managers
#include "account_manager.hpp"
#include "character_manager.hpp"
#include "client_manager.hpp"
#include "room_manager.hpp"

//utilities
#include "config_utility.hpp"
#include "udp_network_utility.hpp"

//common utilities
#include "ip_operators.hpp"
#include "serial_packet.hpp"
#include "singleton.hpp"

//server utilities
#include "server_utilities.hpp"

//APIs
#include "lua.hpp"
#include "sqlite3.h"

#include "SDL/SDL.h"

//STL
#include <map>
#include <string>

//The main application class
class ServerApplication: public Singleton<ServerApplication> {
public:
	//public methods
	void Init(int argc, char* argv[]);
	void Proc();
	void Quit();

private:
	friend Singleton<ServerApplication>;

	ServerApplication() = default;
	~ServerApplication() = default;

	//handle incoming traffic
	void HandlePacket(SerialPacket* const);

	//heartbeat system
	void hPing(ServerPacket* const);
	void hPong(ServerPacket* const);

	//basic connections
	void hBroadcastRequest(ServerPacket* const);
	void hJoinRequest(ClientPacket* const);
	void hLoginRequest(ClientPacket* const);

	//client disconnections
	void hLogoutRequest(ClientPacket* const);
	void hDisconnectRequest(ClientPacket* const);

	//server commands
	void hAdminDisconnectForced(ClientPacket* const);
	void hAdminShutdownRequest(ClientPacket* const);

	//data management
	void hRegionRequest(RegionPacket* const);
	void hQueryCharacterExists(CharacterPacket* const);
	void hQueryCharacterStats(CharacterPacket* const);
	void hQueryCharacterLocation(CharacterPacket* const);
	void hQueryMonsterExists(MonsterPacket* const);
	void hQueryMonsterStats(MonsterPacket* const);
	void hQueryMonsterLocation(MonsterPacket* const);

	//character management
	void hCharacterCreate(CharacterPacket* const);
	void hCharacterDelete(CharacterPacket* const);
	void hCharacterLoad(CharacterPacket* const);
	void hCharacterUnload(CharacterPacket* const);

	//character movement
	void hCharacterMovement(CharacterPacket* const);
	void hCharacterAttack(CharacterPacket* const);
	void hCharacterDamage(CharacterPacket* const);

	//chat
	void hTextBroadcast(TextPacket* const);
	void hTextSpeech(TextPacket* const);
	void hTextWhisper(TextPacket* const);

	//utility methods
	void SaveServerState();

	//APIs and utilities
	sqlite3* database = nullptr;
	lua_State* luaState = nullptr;

	//ugly references; I hate this
	ClientManager& clientMgr = ClientManager::GetSingleton();
	AccountManager& accountMgr = AccountManager::GetSingleton();
	CharacterManager& characterMgr = CharacterManager::GetSingleton();
	RoomManager& roomMgr = RoomManager::GetSingleton();

	ConfigUtility& config = ConfigUtility::GetSingleton();
	UDPNetworkUtility& network = UDPNetworkUtility::GetSingleton();

	//misc
	bool running = true;
};
