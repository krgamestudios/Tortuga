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

//-------------------------
//basic connections
//-------------------------

void ServerApplication::HandlePing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void ServerApplication::HandlePong(ServerPacket* const argPacket) {
	//find and update the specified client
	for (auto& it : clientMap) {
		if (it.second.GetAddress().host == argPacket->srcAddress.host &&
			it.second.GetAddress().port == argPacket->srcAddress.port
			) {
			it.second.ResetAttempts();
			break;
		}
	}
}

void ServerApplication::HandleBroadcastRequest(ServerPacket* const argPacket) {
	//send the server's data
	ServerPacket newPacket;

	newPacket.type = SerialPacketType::BROADCAST_RESPONSE;
	strncpy(newPacket.name, config["server.name"].c_str(), PACKET_STRING_SIZE);
	newPacket.playerCount = characterMgr.GetLoadedCount();
	newPacket.version = NETWORK_VERSION;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::HandleJoinRequest(ClientPacket* const argPacket) {
	//load the user account
	//TODO: handle passwords
	int accountIndex = accountMgr.Load(argPacket->username, clientIndex);

	//Cannot load
	if (accountIndex < 0) {
		TextPacket newPacket;
		newPacket.type = SerialPacketType::JOIN_REJECTION;
		std::string msg = std::string() + "Account already loaded: " + argPacket->username;
		memset(newPacket.name, 0, PACKET_STRING_SIZE);
		strncpy(newPacket.text, msg.c_str(), PACKET_STRING_SIZE); //BUG: If the name is too long this would truncate it
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
		return;
	}

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::JOIN_RESPONSE;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

	//register the client
	ClientData newClient;
	newClient.SetAddress(argPacket->srcAddress);
	clientMap[clientIndex++] = newClient;

	//finished this routine
	std::cout << "New connection, " << clientMap.size() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::HandleDisconnect(ClientPacket* const argPacket) {
	//TODO: authenticate who is disconnecting/kicking
	/*Pseudocode:
	if sender's account index -> client index -> address == sender's address then
		continue
	end
	if sender's account index -> admin == true OR sender's account index -> mod == true then
		continue
	end
	if neither of the above is true, then output a warning to the console, and return
	*/

	//forward to the specified client
	network.SendTo(
		clientMap[ accountMgr.Get(argPacket->accountIndex)->GetClientIndex() ].GetAddress(),
		static_cast<SerialPacket*>(argPacket)
	);

	//save and unload this account's characters
	characterMgr.UnloadIf([&](std::pair<int, CharacterData> it) -> bool {
		if (argPacket->accountIndex == it.second.GetOwner()) {
			//pump the unload message to all remaining clients
			PumpCharacterUnload(it.first);
			return true;
		}
		return false;
	});

	//erase the in-memory stuff
	clientMap.erase(accountMgr.Get(argPacket->accountIndex)->GetClientIndex());
	accountMgr.Unload(argPacket->accountIndex);

	//finished this routine
	std::cout << "Disconnection, " << clientMap.size() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::HandleShutdown(ClientPacket* const argPacket) {
	//TODO: authenticate who is shutting the server down
	/*Pseudocode:
	if sender's account -> admin is not true then
		print a warning
		return
	end
	*/

	//end the server
	running = false;

	//disconnect all clients
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT;
	PumpPacket(&newPacket);

	//finished this routine
	std::cout << "Shutdown signal accepted" << std::endl;
}

//-------------------------
//map management
//-------------------------

void ServerApplication::HandleRegionRequest(RegionPacket* const argPacket) {
	RegionPacket newPacket;

	newPacket.type = SerialPacketType::REGION_CONTENT;
	newPacket.roomIndex = argPacket->roomIndex;
	newPacket.x = argPacket->x;
	newPacket.y = argPacket->y;

	//BUG: possibly related to #35
	newPacket.region = roomMgr.Get(argPacket->roomIndex)->GetPager()->GetRegion(argPacket->x, argPacket->y);

	//send the content
	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

//-------------------------
//Character Management
//-------------------------

void ServerApplication::HandleCharacterNew(CharacterPacket* const argPacket) {
	//NOTE: misnomer, try to load the character first
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	//cannot load or create
	if (characterIndex < 0) {
		//build the error message
		std::string msg;
		if (characterIndex == -1) {
			msg += "Character already loaded: ";
		}
		else if (characterIndex == -2) {
			msg += "Character already exists: ";
		}
		msg += argPacket->handle;

		//create, fill and send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		memset(newPacket.name, 0, PACKET_STRING_SIZE);
		strncpy(newPacket.text, msg.c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
		return;
	}

	//send this new character to all clients
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_NEW;
	CopyCharacterToPacket(&newPacket, characterIndex);
	PumpPacket(&newPacket);
}

void ServerApplication::HandleCharacterDelete(CharacterPacket* const argPacket) {
	//NOTE: Disconnecting only unloads a character, this explicitly deletes it

	//Authenticate the owner is doing this
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	//if this is not your character
	if (characterIndex < 0 && characterMgr.Get(characterIndex)->GetOwner() != argPacket->accountIndex) {
		//send the rejection packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		memset(newPacket.name, 0, PACKET_STRING_SIZE);
		strncpy(newPacket.text, "Character cannot be deleted", PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		//unload an unneeded character
		if (characterIndex != -1) {
			characterMgr.Unload(characterIndex);
		}
		return;
	}

	//delete it
	characterMgr.Delete(characterIndex);

	//TODO: success packet

	//Unload this character from all clients
	PumpCharacterUnload(characterIndex);
}

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

	*character->GetBaseStats() = argPacket->stats;

	//TODO: gameplay components: equipment, items, buffs, debuffs

	PumpPacket(argPacket);
}

//-------------------------
//mismanagement
//-------------------------

void ServerApplication::HandleSynchronize(ClientPacket* const argPacket) {
	//TODO: compensate for large distances
	//NOTE: I quite dislike this function

	//send all of the server's data to this client
	ClientData& client = clientMap[argPacket->clientIndex];

	//send all characters
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_UPDATE;

	for (auto& it : *characterMgr.GetContainer()) {
		CopyCharacterToPacket(&newPacket, it.first);
		network.SendTo(client.GetAddress(), static_cast<SerialPacket*>(&newPacket));
	}

	//TODO: more in HandleSynchronize()
}

//-------------------------
//utility methods
//-------------------------

void ServerApplication::CheckClientConnections() {
	for (auto& it : clientMap) {
		if (std::chrono::steady_clock::now() - it.second.GetLastBeat() > std::chrono::seconds(3)) {
			ServerPacket newPacket;
			newPacket.type = SerialPacketType::PING;
			network.SendTo(it.second.GetAddress(), &newPacket);
			it.second.IncrementAttempts();
		}

		if (it.second.GetAttempts() > 2) {
			CleanupLostConnection(it.first);
			//all iterators are invalid, so we can't continue
			break;
		}
	}
}

void ServerApplication::CleanupLostConnection(int clientIndex) {
	//NOTE: This assumes each player has only one account and character at a time
	//TODO: handle multiple characters (bots, etc.)

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

	//send a disconnection message just in case
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT;
	network.SendTo(clientMap[clientIndex].GetAddress(), &newPacket);

	//clean up this mess
	characterMgr.Unload(characterIndex);
	accountMgr.Unload(accountIndex);
	clientMap.erase(clientIndex);

	PumpCharacterUnload(characterIndex);

	//output a message
	std::cerr << "Connection lost: " << std::endl;
	std::cerr << "\tClient: " << clientIndex << std::endl;
	std::cerr << "\tAccount: " << accountIndex << std::endl;
	std::cerr << "\tCharacter: " << characterIndex << std::endl;
	std::cout << clientMap.size() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

//TODO: a function that only sends to characters in a certain proximity

void ServerApplication::PumpPacket(SerialPacket* const argPacket) {
	for (auto& it : clientMap) {
		network.SendTo(it.second.GetAddress(), argPacket);
	}
}

void ServerApplication::PumpCharacterUnload(int uid) {
	//delete the client-side character(s)
	//NOTE: This is a strange function
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = uid;
	PumpPacket(static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::CopyCharacterToPacket(CharacterPacket* const packet, int characterIndex) {
	CharacterData* character = characterMgr.Get(characterIndex);
	if (!character) {
		throw(std::runtime_error("Failed to copy a character to a packet"));
	}

	//TODO: keep this up to date when the character changes
	packet->characterIndex = characterIndex;
	strncpy(packet->handle, character->GetHandle().c_str(), PACKET_STRING_SIZE);
	strncpy(packet->avatar, character->GetAvatar().c_str(), PACKET_STRING_SIZE);
	packet->accountIndex = character->GetOwner();
	packet->roomIndex = character->GetRoomIndex();
	packet->origin = character->GetOrigin();
	packet->motion = character->GetMotion();
	packet->stats = *character->GetBaseStats();
}