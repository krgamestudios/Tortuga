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

#include <iostream>

//-------------------------
//basic connections
//-------------------------

void ServerApplication::HandlePing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(&argPacket->srcAddress, &newPacket);
}

void ServerApplication::HandlePong(ServerPacket* const argPacket) {
	//TODO: ServerApplications::HandlePong()
}

void ServerApplication::HandleBroadcastRequest(ServerPacket* const argPacket) {
	//send the server's data
	ServerPacket newPacket;

	newPacket.type = SerialPacketType::BROADCAST_RESPONSE;
	strncpy(newPacket.name, config["server.name"].c_str(), PACKET_STRING_SIZE);
	newPacket.playerCount = characterMgr.GetContainer()->size();
	newPacket.version = NETWORK_VERSION;

	network.SendTo(&argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::HandleJoinRequest(ClientPacket* const argPacket) {
	//create the new client
	ClientData newClient;
	newClient.address = argPacket->srcAddress;

	//load the user account
	//TODO: handle passwords
	int accountIndex = accountMgr.LoadAccount(argPacket->username, clientIndex);
	if (accountIndex < 0) {
		//TODO: send rejection packet
		std::cerr << "Error: Account already loaded: " << accountIndex << std::endl;
		return;
	}

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::JOIN_RESPONSE;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(&newClient.address, static_cast<SerialPacket*>(&newPacket));

	//finished this routine
	clientMap[clientIndex++] = newClient;
	std::cout << "New connection, " << clientMap.size() << " clients and " << accountMgr.GetContainer()->size() << " accounts total" << std::endl;
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
		&clientMap[ accountMgr.GetAccount(argPacket->accountIndex)->GetClientIndex() ].address,
		static_cast<SerialPacket*>(argPacket)
	);

	//save and unload this account's characters
	//pump the unload message to all remaining clients
	characterMgr.UnloadCharacterIf([&](std::map<int, CharacterData>::iterator it) -> bool {
		if (argPacket->accountIndex == it->second.GetOwner()) {
			PumpCharacterUnload(it->first);
			return true;
		}
		return false;
	});

	//erase the in-memory stuff
	clientMap.erase(accountMgr.GetAccount(argPacket->accountIndex)->GetClientIndex());
	accountMgr.UnloadAccount(argPacket->accountIndex);

	//finished this routine
	std::cout << "Disconnection, " << clientMap.size() << " clients and " << accountMgr.GetContainer()->size() << " accounts total" << std::endl;
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

	newPacket.region = roomMgr.GetRoom(argPacket->roomIndex)->GetPager()->GetRegion(argPacket->x, argPacket->y);

	//send the content
	network.SendTo(&argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

//-------------------------
//Character Management
//-------------------------

void ServerApplication::HandleCharacterNew(CharacterPacket* const argPacket) {
	//BUG: #27 Characters can be created with an invalid account index
	//NOTE: misnomer, try to load the character first
	int characterIndex = characterMgr.LoadCharacter(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex == -1) {
		//TODO: rejection packet
		std::cerr << "Warning: Character already loaded" << std::endl;
		return;
	}

	if (characterIndex == -2) {
		//TODO: rejection packet
		std::cerr << "Warning: Character already exists" << std::endl;
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
	int characterIndex = characterMgr.LoadCharacter(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	//if this is not your character
	if (characterIndex == -2) {
		//TODO: rejection packet
		std::cerr << "Warning: Character cannot be deleted" << std::endl;

		//unload an unneeded character
		if (characterIndex != -1) {
			characterMgr.UnloadCharacter(characterIndex);
		}
		return;
	}

	//delete it
	characterMgr.DeleteCharacter(characterIndex);

	//TODO: success packet

	//Unload this character from all clients
	PumpCharacterUnload(characterIndex);
}

void ServerApplication::HandleCharacterUpdate(CharacterPacket* const argPacket) {
	CharacterData* character = characterMgr.GetCharacter(argPacket->characterIndex);

	//make a new character if this one doesn't exist
	if (!character) {
		//this isn't normal
		std::cerr << "Warning: HandleCharacterUpdate() is passing to HandleCharacterNew()" << std::endl;
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
		newPacket.characterIndex = it.first;
		CopyCharacterToPacket(&newPacket, it.first);
		network.SendTo(&client.address, static_cast<SerialPacket*>(&newPacket));
	}

	//TODO: more in HandleSynchronize()
}

//-------------------------
//utility methods
//-------------------------

//TODO: a function that only sends to characters in a certain proximity

void ServerApplication::PumpPacket(SerialPacket* const argPacket) {
	for (auto& it : clientMap) {
		network.SendTo(&it.second.address, argPacket);
	}
}

void ServerApplication::PumpCharacterUnload(int uid) {
	//delete the client-side character(s)
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = uid;
	PumpPacket(static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::CopyCharacterToPacket(CharacterPacket* const packet, int characterIndex) {
	CharacterData* character = characterMgr.GetCharacter(characterIndex);
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