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

//TODO: client_manager.cpp

int ClientManager::Create(IPaddress add) {
	//
}

int ClientManager::Load(IPaddress add) {
	//
}

int ClientManager::Save(int uid) {
	//
}

void ClientManager::Unload(int uid) {
	//
}

void ClientManager::Delete(int uid) {
	//
}

void ClientManager::UnloadAll() {
	//
}

void ClientManager::UnloadIf(std::function<bool(std::pair<const int, ClientData>)> fn) {
	//
}

ClientData* ClientManager::Get(int uid) {
	//
}

int ClientManager::GetLoadedCount() {
	//
}

int ClientManager::GetTotalCount() {
	//
}

std::map<int, ClientData>* ClientManager::GetContainer() {
	//
}