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
#include "server_utility.hpp"
#include "client_entry.hpp"
#include "player_entry.hpp"

//maps
#include "map_allocator.hpp"
#include "map_file_format.hpp"
#include "region_pager.hpp"

//networking
#include "serial_packet.hpp"
#include "udp_network_utility.hpp"
#include "serial.hpp"

//common
#include "config_utility.hpp"
#include "vector2.hpp"

//APIs
#include "lua/lua.hpp"
#include "sqlite3/sqlite3.h"
#include "SDL/SDL.h"

//STL
#include <map>

//The main application class
class ServerApplication {
public:
	//standard functions
	ServerApplication() = default;
	~ServerApplication() = default;

	void Init(int argc, char** argv);
	void Proc();
	void Quit();

private:
	void HandlePacket(SerialPacket);

	//high cohesion utility functions
	void HandleBroadcastRequest(SerialPacket);
	void HandleJoinRequest(SerialPacket);
	void HandleDisconnect(SerialPacket);
	void HandleSynchronize(SerialPacket);
	void HandleShutdown(SerialPacket);
	void HandlePlayerNew(SerialPacket);
	void HandlePlayerDelete(SerialPacket);
	void HandlePlayerUpdate(SerialPacket);
	void HandleRegionRequest(SerialPacket);

	//TODO: a function that sends to players in a certain proximity
	void PumpPacket(SerialPacket);

	//APIs
	UDPNetworkUtility network;
	sqlite3* database = nullptr;
	lua_State* luaState = nullptr;

	//server tables
	std::map<unsigned int, ClientEntry> clientMap;
	std::map<unsigned int, PlayerEntry> playerMap;

	//maps
	//TODO: I need to handle multiple map objects
	RegionPager<LuaAllocator, LuaFormat> regionPager;

	//misc
	bool running = true;
	ConfigUtility config;
};

#endif
