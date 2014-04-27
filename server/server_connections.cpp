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
	packet.serverInfo.playerCount = playerMap.size();

	//bounce this packet
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, PACKET_BUFFER_SIZE);
}

void ServerApplication::HandleJoinRequest(SerialPacket packet) {
	//register the new client
	ClientEntry newClient;
	newClient.address = packet.meta.srcAddress;
	clientMap[ClientEntry::uidCounter] = newClient;

	//TODO: move this into the player management code
	//register the new player
	PlayerEntry newPlayer;
	newPlayer.clientIndex = ClientEntry::uidCounter;
	newPlayer.player = packet.clientInfo.player;
	newPlayer.handle = packet.clientInfo.handle;
	newPlayer.avatar = packet.clientInfo.avatar;
	playerMap[PlayerEntry::uidCounter] = newPlayer;

	//send the client their info
	packet.meta.type = SerialPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.clientIndex = ClientEntry::uidCounter;
	packet.clientInfo.playerIndex = PlayerEntry::uidCounter;

	//bounce this packet
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&newClient.address, buffer, PACKET_BUFFER_SIZE);

	//BUG: the new player object is not being sent to existing clients

	//finished this routine
	ClientEntry::uidCounter++;
	PlayerEntry::uidCounter++;
	std::cout << "Connect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleSynchronize(SerialPacket packet) {
	//TODO: compensate for large distances

	//send all the server's data to this client
	SerialPacket newPacket;
	char buffer[PACKET_BUFFER_SIZE];

	//players
	newPacket.meta.type = SerialPacket::Type::PLAYER_UPDATE;
	for (auto& it : playerMap) {
		//TODO: update this for the expanded PlayerEntry structure
		newPacket.playerInfo.playerIndex = it.first;
		snprintf(newPacket.playerInfo.handle, PACKET_STRING_SIZE, "%s", it.second.handle.c_str());
		snprintf(newPacket.playerInfo.avatar, PACKET_STRING_SIZE, "%s", it.second.avatar.c_str());
		newPacket.playerInfo.mapIndex = it.second.mapIndex;
		newPacket.playerInfo.position = it.second.position;
		newPacket.playerInfo.motion = it.second.motion;
		serialize(&newPacket, buffer);
		network.Send(&clientMap[packet.clientInfo.clientIndex].address, buffer, PACKET_BUFFER_SIZE);
	}
}

void ServerApplication::HandleDisconnect(SerialPacket packet) {
	//TODO: authenticate who is disconnecting/kicking
	//TODO: define the difference between unloading and deletng a player

	//disconnect the specified client
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&clientMap[packet.clientInfo.clientIndex].address, buffer, PACKET_BUFFER_SIZE);
	clientMap.erase(packet.clientInfo.clientIndex);

	//prep the delete packet
	SerialPacket delPacket;
	delPacket.meta.type = SerialPacket::Type::PLAYER_DELETE;

	//delete server and client side players
	erase_if(playerMap, [&](std::pair<int, PlayerEntry> it) -> bool {
		//find the internal players to delete
		if (it.second.clientIndex == packet.clientInfo.clientIndex) {
			//send the delete player command to all clients
			delPacket.playerInfo.playerIndex = it.first;
			PumpPacket(delPacket);

			//delete this player object
			return true;
		}

		//don't delete this player object
		return false;
	});

	//finished this routine
	std::cout << "Disconnect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleShutdown(SerialPacket packet) {
	//TODO: authenticate who is shutting the server down

	//end the server
	running = false;

	//disconnect all clients
	packet.meta.type = SerialPacket::Type::DISCONNECT;
	packet.clientInfo.clientIndex = -1;
	PumpPacket(packet);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

void ServerApplication::HandlePlayerUpdate(SerialPacket packet) {
	//TODO: this should be moved elsewhere
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot update a non-existant player"));
	}

	//TODO: the server needs it's own movement system too
	playerMap[packet.playerInfo.playerIndex].position = packet.playerInfo.position;
	playerMap[packet.playerInfo.playerIndex].motion = packet.playerInfo.motion;

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
