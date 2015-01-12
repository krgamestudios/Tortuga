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
#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

//managers
#include "account_manager.hpp"
#include "character_manager.hpp"
#include "client_manager.hpp"
#include "monster_manager.hpp"
#include "room_manager.hpp"

//utilities
#include "config_utility.hpp"
#include "udp_network_utility.hpp"

//common utilities
#include "serial_packet.hpp"
#include "singleton.hpp"

//APIs
#include "lua.hpp"
#include "sqlite3.h"

#include "SDL/SDL.h"

//STL
#include <map>
#include <string>

//global utility functions
bool operator==(IPaddress lhs, IPaddress rhs);
bool operator!=(IPaddress lhs, IPaddress rhs);

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

	//heartbeat sustem
	void HandlePing(ServerPacket* const);
	void HandlePong(ServerPacket* const);

	//basic connections
	void HandleBroadcastRequest(ServerPacket* const);
	void HandleJoinRequest(ClientPacket* const);
	void HandleLoginRequest(ClientPacket* const);

	//client disconnections
	void HandleLogoutRequest(ClientPacket* const);
	void HandleDisconnectRequest(ClientPacket* const);

	//server commands
	void HandleDisconnectForced(ClientPacket* const);
	void HandleShutdownRequest(ClientPacket* const);

	//data management
	void HandleRegionRequest(RegionPacket* const);
	void HandleCharacterExists(CharacterPacket* const);

	void SaveServerState();
	void FullClientUnload(int index);
	void FullAccountUnload(int index);
	void FullCharacterUnload(int index);

	//character management
	void HandleCharacterCreate(CharacterPacket* const);
	void HandleCharacterDelete(CharacterPacket* const);
	void HandleCharacterLoad(CharacterPacket* const);
	void HandleCharacterUnload(CharacterPacket* const);

	//character movement
	void HandleCharacterSetRoom(CharacterPacket* const);
	void HandleCharacterSetOrigin(CharacterPacket* const);
	void HandleCharacterSetMotion(CharacterPacket* const);

	//utility methods
	void PumpPacket(SerialPacket* const);
	void PumpPacketProximity(SerialPacket* const argPacket, int roomIndex, Vector2 position, int distance);
	void CopyCharacterToPacket(CharacterPacket* const packet, int characterIndex);

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

#endif
