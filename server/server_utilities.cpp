/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "server_utilities.hpp"

#include "account_manager.hpp"
#include "character_manager.hpp"
#include "client_manager.hpp"
#include "room_manager.hpp"
#include "udp_network_utility.hpp"

#include <cstring>

/*DOCS: each of these calls the next (diagram)
	fullClientUnload()
		-> fullAccountUnload()
			-> fullCharacterUnload()
				-> pumpPacketProximity()
	
	copyCharacterToPacket() v1
		-> copyCharacterToPacket() v2
*/

//-------------------------
//manager unload functions
//-------------------------

void fullClientUnload(int index) {
	ClientManager::GetSingleton().UnloadIf([index](std::pair<const int, ClientData const&> clientPair) -> bool {
		//skip the wrong clients
		if (clientPair.first != index) {
			return false;
		}

		AccountManager& accountMgr = AccountManager::GetSingleton();

		//unload associated accounts
		for (std::map<int, AccountData>::iterator it = accountMgr.GetContainer()->begin(); it != accountMgr.GetContainer()->end(); /* EMPTY */) {
			if (it->second.GetClientIndex() == index) {
				fullAccountUnload(it->first);
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

void fullAccountUnload(int index) {
	AccountManager::GetSingleton().UnloadIf([index](std::pair<const int, AccountData const&> accountPair) -> bool {
		//skip the wrong accounts
		if (accountPair.first != index) {
			return false;
		}

		CharacterManager& characterMgr = CharacterManager::GetSingleton();

		//unload associated characters
		for (std::map<int, CharacterData>::iterator it = characterMgr.GetContainer()->begin(); it != characterMgr.GetContainer()->end(); /* EMPTY */) {
			if (it->second.GetOwner() == index) {
				fullCharacterUnload(it->first);
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

void fullCharacterUnload(int index) {
	CharacterManager::GetSingleton().UnloadIf([index](std::pair<const int, CharacterData const&> characterPair) -> bool {
		//skip the wrong characters
		if (characterPair.first != index) {
			return false;
		}

		//pop from the rooms
		RoomManager::GetSingleton().PopCharacter(&characterPair.second);

		//pump character unload
		CharacterPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_UNLOAD;
		newPacket.characterIndex = characterPair.first;
		//NOTE: more character info as needed

		//TODO: proximity?
		pumpPacketProximity(&newPacket, characterPair.second.GetRoomIndex());

		//unload this character
		return true;
	});
}

//-------------------------
//utility functions
//-------------------------

void pumpPacket(SerialPacket* const argPacket) {
	//send a message to all clients
	for (auto& it : *ClientManager::GetSingleton().GetContainer()) {
		UDPNetworkUtility::GetSingleton().SendTo(it.second.GetAddress(), argPacket);
	}
}

void pumpPacketProximity(SerialPacket* const argPacket, int roomIndex, Vector2 position, int distance) {
	//send this packet to all characters within a certain distance of a point in a room
	RoomData* roomData = RoomManager::GetSingleton().Find(roomIndex);

	if (!roomData) {
		throw(std::runtime_error("Failed to pump to a non-existant room"));
	}

	AccountData* accountData = nullptr;
	ClientData* clientData = nullptr;

	for (auto& characterIt : *roomData->GetCharacterList()) {
		if (distance == -1 || (characterIt->GetOrigin() - position).Length() <= distance) {
			accountData = AccountManager::GetSingleton().Find(characterIt->GetOwner());
			clientData = ClientManager::GetSingleton().Find(accountData->GetClientIndex());
			UDPNetworkUtility::GetSingleton().SendTo(clientData->GetAddress(), argPacket);
		}
	}
}

void copyCharacterToPacket(CharacterPacket* const packet, int characterIndex) {
	CharacterData* characterData = CharacterManager::GetSingleton().Find(characterIndex);
	if (!characterData) {
		throw(std::runtime_error("Failed to copy a character to a packet"));
	}

	copyCharacterToPacket(packet, characterData, characterIndex);
}

void copyCharacterToPacket(CharacterPacket* const packet, CharacterData* const characterData, int characterIndex) {
	//NOTE: keep this up to date when the character changes
	packet->characterIndex = characterIndex;
	strncpy(packet->handle, characterData->GetHandle().c_str(), PACKET_STRING_SIZE);
	strncpy(packet->avatar, characterData->GetAvatar().c_str(), PACKET_STRING_SIZE);
	packet->accountIndex = characterData->GetOwner();
	packet->roomIndex = characterData->GetRoomIndex();
	packet->origin = characterData->GetOrigin();
	packet->motion = characterData->GetMotion();
	packet->bounds = characterData->GetBounds();
}

void copyCreatureToPacket(CreaturePacket* const packet, CreatureData* const creatureData, int creatureIndex) {
	packet->creatureIndex = creatureIndex;
	strncpy(packet->avatar, creatureData->GetAvatar().c_str(), PACKET_STRING_SIZE);
	packet->origin = creatureData->GetOrigin();
	packet->motion = creatureData->GetMotion();
	packet->bounds = creatureData->GetBounds();
}

void copyBarrierToPacket(BarrierPacket* const packet, BarrierData* const barrierData, int barrierIndex) {
	packet->barrierIndex = barrierIndex;
	packet->roomIndex = barrierData->GetRoomIndex();
	packet->origin = barrierData->GetOrigin();
	packet->motion = barrierData->GetMotion();
	packet->bounds = barrierData->GetBounds();

	memcpy(packet->status, barrierData->GetStatusArray(), sizeof(int) * 8);
}

void pumpAndChangeRooms(int characterIndex, int newRoomIndex) {
	//get the character object
	CharacterData* character = CharacterManager::GetSingleton().Find(characterIndex);

	//pass ownwards
	pumpAndChangeRooms(character, newRoomIndex, characterIndex);
}

//TODO: (0) refactor this
void pumpAndChangeRooms(CharacterData* const characterData, int newRoomIndex, int characterIndex) {
	//delete the character from the old room
	CharacterPacket newPacket;
	copyCharacterToPacket(&newPacket, characterData, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_UNLOAD;
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());

	//move the character between rooms
	RoomManager::GetSingleton().PopCharacter(characterData);
	characterData->SetRoomIndex(newRoomIndex);
	RoomManager::GetSingleton().PushCharacter(characterData);

	//create character in the new room
	copyCharacterToPacket(&newPacket, characterData, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
}

double distance(Vector2 lhs, Vector2 rhs) {
	return abs((lhs - rhs).Length());
}