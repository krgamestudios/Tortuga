/* Copyright: (c) Kayne Ruse 2013
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

//networking
#include "network_packet.hpp"
#include "udp_network_utility.hpp"
#include "serial.hpp"

//APIs
#include "lua/lua.hpp"
#include "sqlite3/sqlite3.h"
#include "SDL/SDL.h"

//common
#include "config_utility.hpp"
#include "vector2.hpp"

//STL
#include <map>
#include <string>

struct ClientEntry {
	IPaddress address;
};

struct PlayerEntry {
	int clientIndex;
	int mapIndex;
	std::string handle;
	std::string avatar;
	Vector2 position;
	Vector2 motion;
};

//The main application class
class ServerApplication {
public:
	//standard functions
	ServerApplication() = default;
	~ServerApplication() = default;

	void Init(int argc, char** argv);
	void Loop();
	void Quit();

private:
	void HandlePacket(NetworkPacket);

	//high cohesion utility functions
	void HandleBroadcastRequest(NetworkPacket);
	void HandleJoinRequest(NetworkPacket);
	void HandleDisconnect(NetworkPacket);
	void HandleSynchronize(NetworkPacket);
	void HandleShutdown(NetworkPacket);
	void HandlePlayerNew(NetworkPacket);
	void HandlePlayerDelete(NetworkPacket);
	void HandlePlayerUpdate(NetworkPacket);

	void PumpPacket(NetworkPacket);

	//networking
	UDPNetworkUtility network;

	//database
	sqlite3* database = nullptr;

	//lua
	lua_State* luaState = nullptr;

	//misc
	bool running = true;
	ConfigUtility config;

	std::map<int, ClientEntry> clientMap;
	std::map<int, PlayerEntry> playerMap;

	int clientCounter = 0;
	int playerCounter = 0;
};

#endif
