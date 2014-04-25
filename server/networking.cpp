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
	//pack the data
	packet.meta.type = SerialPacket::Type::BROADCAST_RESPONSE;
	packet.serverInfo.networkVersion = NETWORK_VERSION;
	snprintf(packet.serverInfo.name, PACKET_STRING_SIZE, "%s", config["server.name"].c_str());
	packet.serverInfo.playerCount = playerMap.size();

	//send the data
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&packet.meta.srcAddress, buffer, PACKET_BUFFER_SIZE);
}

void ServerApplication::HandleJoinRequest(SerialPacket packet) {
	//register the new client
	ClientEntry newClient;
	newClient.address = packet.meta.srcAddress;
	clientMap[ClientEntry::uidCounter] = newClient;

	//create the new player
	//TODO: make the player

	//send the client their info
	packet.meta.type = SerialPacket::Type::JOIN_RESPONSE;
	packet.clientInfo.index = ClientEntry::uidCounter;

	//bounce this packet
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&newClient.address, buffer, PACKET_BUFFER_SIZE);

	//finished this routine
	ClientEntry::uidCounter++;
	std::cout << "Connect, total: " << clientMap.size() << std::endl;
}

void ServerApplication::HandleDisconnect(SerialPacket packet) {
	//TODO: authenticate who is disconnecting/kicking

	//disconnect the specified client
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(&clientMap[packet.clientInfo.index].address, buffer, PACKET_BUFFER_SIZE);
	clientMap.erase(packet.clientInfo.index);

	//prep the delete packet
	SerialPacket delPacket;
	delPacket.meta.type = SerialPacket::Type::PLAYER_DELETE;

	//TODO: can this use DeletePlayer() instead?
	//delete server and client side players
	erase_if(playerMap, [&](std::pair<int, PlayerEntry> it) -> bool {
		//find the internal players to delete
		if (it.second.clientIndex == packet.clientInfo.index) {
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
		newPacket.playerInfo.position = it.second.position;
		newPacket.playerInfo.motion = it.second.motion;
		serialize(&newPacket, buffer);
		network.Send(&clientMap[packet.clientInfo.index].address, buffer, PACKET_BUFFER_SIZE);
	}
}

void ServerApplication::HandleShutdown(SerialPacket packet) {
	//end the server
	running = false;

	//disconnect all clients
	packet.meta.type = SerialPacket::Type::DISCONNECT;
	PumpPacket(packet);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

void ServerApplication::HandlePlayerNew(SerialPacket packet) {
	//register the new PlayerEntry
	//NOTE: assigning each field one-by-one so adding or moving a field doesn't break this code
	PlayerEntry newPlayer;

	//metadata
	newPlayer.clientIndex = packet.playerInfo.clientIndex;
	newPlayer.handle = packet.playerInfo.handle;
	newPlayer.avatar = packet.playerInfo.avatar;

	//position
	newPlayer.mapIndex = 0;
	newPlayer.position = {0,0};
	newPlayer.motion = {0,0};
	newPlayer.bbox = {0, 0, 0, 0};

	//TODO: Add the statistic creation code here

	//push this player
	playerMap[PlayerEntry::uidCounter] = newPlayer;

	//send the client their info
	packet.playerInfo.playerIndex = PlayerEntry::uidCounter;
	packet.playerInfo.position = newPlayer.position;
	packet.playerInfo.motion = newPlayer.motion;

	//actually send to everyone
	PumpPacket(packet);

	//finish this routine
	PlayerEntry::uidCounter++;
}

//TODO: differentiate between delete and unload
void ServerApplication::HandlePlayerDelete(SerialPacket packet) {
	//TODO: authenticate who is deleting this player
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot delete a non-existant player"));
	}

	//TODO: remove the deleted player from the database?

	//prep the delete packet
	SerialPacket delPacket;
	delPacket.meta.type = SerialPacket::Type::PLAYER_DELETE;

	//delete the specified playerEntry
	erase_if(playerMap, [&](std::pair<int, PlayerEntry> it) -> bool {
		//find the specified PlayerEntry
		if (it.first == packet.playerInfo.playerIndex) {
			//send to all
			delPacket.playerInfo.playerIndex = it.first;
			PumpPacket(delPacket);

			//delete this player
			return true;
		}
		//skip this player
		return false;
	});
}

void ServerApplication::HandlePlayerUpdate(SerialPacket packet) {
	if (playerMap.find(packet.playerInfo.playerIndex) == playerMap.end()) {
		throw(std::runtime_error("Cannot update a non-existant player"));
	}

	//TODO: the server needs it's own movement system too
	playerMap[packet.playerInfo.playerIndex].position = packet.playerInfo.position;
	playerMap[packet.playerInfo.playerIndex].motion = packet.playerInfo.motion;

	PumpPacket(packet);
}

void ServerApplication::HandleRegionRequest(SerialPacket packet) {
	char buffer[PACKET_BUFFER_SIZE];
	packet.meta.type = SerialPacket::Type::REGION_CONTENT;
	packet.regionInfo.region = regionPager.GetRegion(packet.regionInfo.x, packet.regionInfo.y);
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
