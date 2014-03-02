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

int ClientManager::HandleConnection(IPaddress add) {
	ClientEntry c;
	c.address = add;
	clientMap[counter] = c;
	return counter++;
}

int ClientManager::HandleDisconnection(int i) {
	for (auto& it : clientMap) {
		if (it.first == i) {
			clientMap.erase(it.first);
			return 0;
		}
	}
	return -1;
}

ClientEntry* ClientManager::GetClient(int i) {
	for (auto& it : clientMap) {
		if (it.first == i) {
			return &it.second;
		}
	}
	return nullptr;
}

ClientEntry* ClientManager::GetClient(IPaddress add) {
	for (auto& it : clientMap) {
		if (it.second.address.host == add.host &&
			it.second.address.port == add.port) {
			return &it.second;
		}
	}
	return nullptr;
}
