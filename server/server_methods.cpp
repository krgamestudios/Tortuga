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
#include "server_application.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

//-------------------------
//these should've come standard
//-------------------------

bool operator==(IPaddress lhs, IPaddress rhs) {
	return lhs.host == rhs.host && lhs.port == rhs.port;
}

bool operator!=(IPaddress lhs, IPaddress rhs) {
	return !(lhs == rhs);
}

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

//-------------------------
//full unload methods
//-------------------------

void ServerApplication::FullClientUnload(int index) {
	clientMgr.UnloadIf([&](std::pair<const int, ClientData const&> client) -> bool {
		//skip the wrong clients
		if (client.first != index) {
			return false;
		}

		//unload associated accounts
		for (std::map<int, AccountData>::iterator it = accountMgr.GetContainer()->begin(); it != accountMgr.GetContainer()->end(); /* EMPTY */) {
			if (it->second.GetClientIndex() == index) {
				FullAccountUnload(it->first);
				it = accountMgr.GetContainer()->begin();
			}
			else {
				++it;
			}
		}

		//unload this client
		return true;
	});
}

void ServerApplication::FullAccountUnload(int index) {
	accountMgr.UnloadIf([&](std::pair<const int, AccountData const&> account) -> bool {
		//skip the wrong accounts
		if (account.first != index) {
			return false;
		}

		//unload associated characters
		for (std::map<int, CharacterData>::iterator it = characterMgr.GetContainer()->begin(); it != characterMgr.GetContainer()->end(); /* EMPTY */) {
			if (it->second.GetOwner() == index) {
				FullCharacterUnload(it->first);
				it = characterMgr.GetContainer()->begin();
			}
			else {
				++it;
			}
		}

		//unload this account
		return true;
	});
}

void ServerApplication::FullCharacterUnload(int index) {
	characterMgr.UnloadIf([&](std::pair<const int, CharacterData const&> character) -> bool {
		//skip the wrong characters
		if (character.first != index) {
			return false;
		}

		//pop from the rooms
		roomMgr.PopEntity(&character.second);

		//pump character unload
		CharacterPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_DELETE;
		newPacket.characterIndex = character.first;
		//NOTE: more character info as needed

		PumpPacket(&newPacket);

		//unload this character
		return true;
	});
}

//-------------------------
//utility methods
//-------------------------

void ServerApplication::PumpPacket(SerialPacket* const argPacket) {
	for (auto& it : *clientMgr.GetContainer()) {
		network.SendTo(it.second.GetAddress(), argPacket);
	}
}

void ServerApplication::CopyCharacterToPacket(CharacterPacket* const packet, int characterIndex) {
	CharacterData* character = characterMgr.Get(characterIndex);
	if (!character) {
		throw(std::runtime_error("Failed to copy a character to a packet"));
	}

	//NOTE: keep this up to date when the character changes
	packet->characterIndex = characterIndex;
	strncpy(packet->handle, character->GetHandle().c_str(), PACKET_STRING_SIZE);
	strncpy(packet->avatar, character->GetAvatar().c_str(), PACKET_STRING_SIZE);
	packet->accountIndex = character->GetOwner();
	packet->roomIndex = character->GetRoomIndex();
	packet->origin = character->GetOrigin();
	packet->motion = character->GetMotion();
}
