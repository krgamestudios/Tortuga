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
#include "client_manager.hpp"

#include "udp_network_utility.hpp"

#include <chrono>

int ClientManager::CheckConnections() {
	for (auto& it : elementMap) {
		//3 seconds between beats
		if (ClientData::Clock::now() - it.second.GetLastBeat() > std::chrono::seconds(3)) {
			ServerPacket newPacket;
			newPacket.type = SerialPacketType::PING;
			UDPNetworkUtility::GetSingleton().SendTo(it.second.GetAddress(), &newPacket);
			it.second.IncrementAttempts();
		}
	}

	for (auto& it : elementMap) {
		if (it.second.GetAttempts() > 2) {
			int ret = it.first;
			elementMap.erase(it.first);
			return ret;
		}
	}

	return -1;
}

void ClientManager::HandlePong(ServerPacket* const argPacket) {
	//find and update the specified client
	for (auto& it : elementMap) {
		if (it.second.GetAddress().host == argPacket->srcAddress.host &&
			it.second.GetAddress().port == argPacket->srcAddress.port
			) {
			it.second.ResetAttempts();
			return;
		}
	}
}

int ClientManager::Create(IPaddress add) {
	ClientData& client = elementMap[counter];
	client.SetAddress(add);
	return counter++;
}

void ClientManager::Unload(int uid) {
	elementMap.erase(uid);
}

void ClientManager::UnloadAll() {
	elementMap.clear();
}

void ClientManager::UnloadIf(std::function<bool(std::pair<const int, ClientData>&)> fn) {
	std::map<int, ClientData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
			//TODO: ? disconnect, unload characters, notify other clients
		}
		else {
			++it;
		}
	}
}

ClientData* ClientManager::Get(int uid) {
	std::map<int, ClientData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int ClientManager::GetLoadedCount() {
	return elementMap.size();
}

int ClientManager::GetTotalCount() {
	return elementMap.size();
}

std::map<int, ClientData>* ClientManager::GetContainer() {
	return &elementMap;
}