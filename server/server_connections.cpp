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
#include "server_application.hpp"

#include <stdexcept>
#include <iostream>

//-------------------------
//Handle various network input
//-------------------------

void ServerApplication::HandleBroadcastRequest(SerialPacket packet) {
	//pack the server's data
	packet.meta.type = SerialPacket::Type::BROADCAST_RESPONSE;
	packet.serverInfo.networkVersion = NETWORK_VERSION;
	snprintf(packet.serverInfo.name, PACKET_STRING_SIZE, "%s", config["server.name"].c_str());
	packet.serverInfo.playerCount = characterMap.size();

	//bounce this packet
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, PACKET_BUFFER_SIZE);
}

void ServerApplication::HandleJoinRequest(SerialPacket packet) {
	//create the new client
	ClientData newClient;
	newClient.address = packet.meta.srcAddress;

	//load the user account
	int accountIndex = LoadUserAccount(packet.clientInfo.username, ClientData::uidCounter);
	if (accountIndex < 0) {
		//TODO: send rejection packet
		std::cerr << "Error: Account already loaded: " << accountIndex << std::endl;
		return;
	}

	//load the new character
	int characterIndex = LoadCharacter(accountIndex, packet.clientInfo.handle, packet.clientInfo.avatar);
	if (characterIndex < 0) {
		//TODO: send rejection packet
		std::cerr << "Error: Character already loaded: " << characterIndex << std::endl;
		UnloadUserAccount(accountIndex);
		return;
	}

	//send the client their info
	packet.meta.type = SerialPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.clientIndex = ClientData::uidCounter;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	//bounce this packet
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&newClient.address, buffer, PACKET_BUFFER_SIZE);

	//send the new character to all clients
	packet.meta.type = SerialPacket::Type::CHARACTER_NEW;
	packet.characterInfo.characterIndex = characterIndex;
	strncpy(packet.characterInfo.handle, characterMap[characterIndex].handle.c_str(), PACKET_STRING_SIZE);
	strncpy(packet.characterInfo.avatar, characterMap[characterIndex].avatar.c_str(), PACKET_STRING_SIZE);
	packet.characterInfo.position = characterMap[characterIndex].position;
	packet.characterInfo.motion = characterMap[characterIndex].motion;
	PumpPacket(packet);

	//TODO: don't send anything to a certain client until they send the OK (the sync packet? or ignore client side?)
	//finished this routine
	clientMap[ClientData::uidCounter++] = newClient;
	std::cout << "Connect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleSynchronize(SerialPacket packet) {
	//TODO: compensate for large distances

	//send all the server's data to this client
	SerialPacket newPacket;
	char buffer[PACKET_BUFFER_SIZE];

	//characters
	newPacket.meta.type = SerialPacket::Type::CHARACTER_UPDATE;
	for (auto& it : characterMap) {
		//TODO: update this for the expanded CharacterData structure
		newPacket.characterInfo.characterIndex = it.first;
		snprintf(newPacket.characterInfo.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(newPacket.characterInfo.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		newPacket.characterInfo.mapIndex = it.second.mapIndex;
		newPacket.characterInfo.position = it.second.position;
		newPacket.characterInfo.motion = it.second.motion;
		serialize(&newPacket, buffer);
		network.Send(&clientMap[packet.clientInfo.clientIndex].address, buffer, PACKET_BUFFER_SIZE);
	}
}

void ServerApplication::HandleDisconnect(SerialPacket packet) {
	//TODO: authenticate who is disconnecting/kicking

	//forward to the specified client
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&clientMap[accountMap[packet.clientInfo.accountIndex].clientIndex].address, buffer, PACKET_BUFFER_SIZE);

	//unload client and server-side characters
	for (std::map<int, CharacterData>::iterator it = characterMap.begin(); it != characterMap.end(); /* EMPTY */ ) {
		if (it->second.owner == packet.clientInfo.accountIndex) {
			PumpCharacterUnload(it->first);
			SaveCharacter(it->first);
			it = characterMap.erase(it); //efficient
			continue;
		}
		else {
			++it;
		}
	}

	//erase the in-memory stuff
	clientMap.erase(accountMap[packet.clientInfo.accountIndex].clientIndex);
	UnloadUserAccount(packet.clientInfo.accountIndex);

	//finished this routine
	std::cout << "Disconnect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleShutdown(SerialPacket packet) {
	//TODO: authenticate who is shutting the server down

	//end the server
	running = false;

	//disconnect all clients
	packet.meta.type = SerialPacket::Type::DISCONNECT;
	PumpPacket(packet);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

void ServerApplication::HandleCharacterUpdate(SerialPacket packet) {
	//TODO: this should be moved elsewhere
	if (characterMap.find(packet.characterInfo.characterIndex) == characterMap.end()) {
		throw(std::runtime_error("Cannot update a non-existant character"));
	}

	//TODO: the server needs it's own movement system too
	characterMap[packet.characterInfo.characterIndex].position = packet.characterInfo.position;
	characterMap[packet.characterInfo.characterIndex].motion = packet.characterInfo.motion;

	PumpPacket(packet);
}

void ServerApplication::HandleRegionRequest(SerialPacket packet) {
	//TODO: this should be moved elsewhere
	packet.meta.type = SerialPacket::Type::REGION_CONTENT;
	packet.regionInfo.region = regionPager.GetRegion(packet.regionInfo.x, packet.regionInfo.y);

	//send the content
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, PACKET_BUFFER_SIZE);
}

void ServerApplication::PumpPacket(SerialPacket packet) {
	//NOTE: I don't really like this, but it'll do for now
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	for (auto& it : clientMap) {
		network.Send(&it.second.address, buffer, PACKET_BUFFER_SIZE);
	}
}

void ServerApplication::PumpCharacterUnload(int uid) {
	//delete the client-side character(s)
	SerialPacket delPacket;
	delPacket.meta.type = SerialPacket::Type::CHARACTER_DELETE;
	delPacket.characterInfo.characterIndex = uid;
	PumpPacket(delPacket);
}