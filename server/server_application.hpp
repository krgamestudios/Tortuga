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

#include "packet.hpp"
#include "singleton.hpp"
#include "network_queue.hpp"

#include "config_Utility.hpp"
#include "udp_network_utility.hpp"
#include "vector2.hpp"

#include "SDL/SDL_thread.h"

#include <map>
#include <string>
#include <algorithm>

struct ClientEntry {
	int index;
	IPaddress address;
};

struct PlayerEntry {
	int index;
	int clientIndex;
	std::string handle;
	std::string avatar;
	Vector2 position;
	Vector2 motion;
};

class ServerApplication {
public:
	ServerApplication();
	~ServerApplication();
	void Init();
	void Proc();
	void Quit();

	ServerApplication(ServerApplication const&) = delete;
	ServerApplication(ServerApplication const&&) = delete;
private:
	//game loop
	void UpdateWorld(double delta);

	//network loop
	int HandlePacket(Packet);
	void RelayPacket(Packet&);

	void SynchronizeEverything(Packet&);

	void HandleBroadcast(Packet&);
	void HandleConnection(Packet&);
	void HandleDisconnection(Packet&);

	void AddPlayer(Packet&);
	void RemovePlayer(Packet&);
	void UpdatePlayer(Packet&);

	//services
	ConfigUtility* configUtil = Singleton<ConfigUtility>::Get();
	UDPNetworkUtility* netUtil = Singleton<UDPNetworkUtility>::Get();

	//members
	std::map<int, ClientEntry> clients;
	std::map<int, PlayerEntry> players;

	bool running = false;

	int uniqueIndex = 0;
};

#endif
