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
#ifndef CLIENTMANAGER_HPP_
#define CLIENTMANAGER_HPP_

#include "client_data.hpp"
#include "server_packet.hpp"
#include "singleton.hpp"

#include "SDL_net.h"

#include <functional>
#include <list>
#include <map>

class ClientManager: public Singleton<ClientManager> {
public:
	//methods
	std::list<int> CheckConnections();
	void HandlePong(ServerPacket* const argPacket);

	//common public methods
	int Create(IPaddress);
	void Unload(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, ClientData const&>)> fn);

	//accessors & mutators
	ClientData* Get(int uid);
	int GetLoadedCount();
	int GetTotalCount();
	std::map<int, ClientData>* GetContainer();

private:
	friend Singleton<ClientManager>;

	ClientManager() = default;
	~ClientManager() = default;

	//members
	std::map<int, ClientData> elementMap;
	int counter = 0;
};

#endif