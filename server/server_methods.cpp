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
#include "server_application.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

//-------------------------
//server commands
//-------------------------

void ServerApplication::HandleShutdownRequest(ClientPacket* const argPacket) {
	//get the account and client data
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	if (!accountData) {
		return;
	}
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		std::ostringstream msg;
		msg << "No client found for an account: " << accountData->GetUsername();
		throw(std::logic_error(msg.str()));
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified server shutdown detected from: " << accountData->GetUsername() << std::endl;
		return;
	}

	//reject non-admin requests
	if (accountData->GetAdministrator() != true) {
		std::cerr << "Rejected server shutdown command from: " << accountData->GetUsername() << std::endl;

		//build the message
		std::ostringstream msg;
		msg << "Invalid admin status";

		//build the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::SHUTDOWN_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);

		//send the rejection message
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//end the server
	running = false;

	//disconnect all clients
	TextPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT_FORCED;
	strncpy(newPacket.text, "Server shutdown", PACKET_STRING_SIZE);
	PumpPacket(&newPacket);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

/*

//SET: entities
void ServerApplication::HandleCharacterUpdate(CharacterPacket* const argPacket) {
	CharacterData* character = characterMgr.Get(argPacket->characterIndex);

	//make a new character if this one doesn't exist
	if (!character) {
		HandleCharacterNew(argPacket);
		return;
	}

	//accept client-side logic
	character->SetRoomIndex(argPacket->roomIndex);
	character->SetOrigin(argPacket->origin);
	character->SetMotion(argPacket->motion);

	//TODO: gameplay components: equipment, items, buffs, debuffs

	PumpPacket(argPacket);
}

//-------------------------
//mismanagement
//-------------------------

//SET: delete
void ServerApplication::HandleSynchronize(ClientPacket* const argPacket) {
	//TODO: compensate for large distances
	//NOTE: I quite dislike this function

	//send all of the server's data to this client
	ClientData* client = clientMgr.Get(argPacket->clientIndex);

	//send all characters
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_SET_ORIGIN;

	for (auto& it : *characterMgr.GetContainer()) {
		CopyCharacterToPacket(&newPacket, it.first);
		network.SendTo(client->GetAddress(), static_cast<SerialPacket*>(&newPacket));
	}

	//TODO: more in HandleSynchronize()
}

//SET: utility/manager
void ServerApplication::CleanupLostConnection(int clientIndex) {
	//NOTE: This assumes each player has only one account and character at a time
	//TODO: handle multiple characters (bots, etc.)

	//send a disconnection message just in case
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT_FORCED;
	network.SendTo(clientMgr.Get(clientIndex)->GetAddress(), &newPacket);

	//find the account
	int accountIndex = -1;
	for (auto& it : *accountMgr.GetContainer()) {
		if (it.second.GetClientIndex() == clientIndex) {
			accountIndex = it.first;
			break;
		}
	}

	//find the character
	int characterIndex = -1;
	for (auto& it : *characterMgr.GetContainer()) {
		if (it.second.GetOwner() == accountIndex) {
			characterIndex = it.first;
			break;
		}
	}

	//clean up this mess
	characterMgr.Unload(characterIndex);
	accountMgr.Unload(accountIndex);
	clientMgr.Unload(clientIndex);

	PumpCharacterUnload(characterIndex);

	//output a message
	std::cerr << "Connection lost: " << std::endl;
	std::cerr << "\tClient: " << clientIndex << std::endl;
	std::cerr << "\tAccount: " << accountIndex << std::endl;
	std::cerr << "\tCharacter: " << characterIndex << std::endl;
	std::cout << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

//TODO: remove this terminate comment
//*/