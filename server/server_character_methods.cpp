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

#include <iostream>
#include <sstream>

//-------------------------
//Character Management
//-------------------------

void ServerApplication::hCharacterCreate(CharacterPacket* const argPacket) {
	int characterIndex = characterMgr.Create(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		msg << "Character already exists: " << argPacket->handle;

		//build & send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//push to the rooms
	CharacterData* characterData = characterMgr.Get(characterIndex);
	roomMgr.PushCharacter(characterData);

	//pump this character to all clients
	CharacterPacket newPacket;
	copyCharacterToPacket(&newPacket, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
}

void ServerApplication::hCharacterDelete(CharacterPacket* const argPacket) {
	//get the user's data
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	if (!accountData) {
		return;
	}
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		return;
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified character deletion detected targeting: " << argPacket->handle << std::endl;
		return;
	}

	//load the character into memory
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		msg << "Cannot delete this character";

		//build & send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//pop from the rooms
	CharacterData* characterData = characterMgr.Get(characterIndex);
	roomMgr.PopCharacter(characterData);

	//pump character delete
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = characterIndex;
	pumpPacketProximity(static_cast<SerialPacket*>(&newPacket), characterData->GetRoomIndex());

	//delete the character
	characterMgr.Delete(characterIndex);
}

void ServerApplication::hCharacterLoad(CharacterPacket* const argPacket) {
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		if (characterIndex == -1) {
			msg << "Character already loaded: ";
		}
		if (characterIndex == -2) {
			msg << "Character name is taken: ";
		}
		msg << argPacket->handle;

		//build & send the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::CHARACTER_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

		return;
	}

	//push to the rooms
	CharacterData* characterData = characterMgr.Get(characterIndex);
	roomMgr.PushCharacter(characterData);

	//pump this character to all clients
	CharacterPacket newPacket;
	copyCharacterToPacket(&newPacket, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
}

void ServerApplication::hCharacterUnload(CharacterPacket* const argPacket) {
	//get the entries
	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);
	if (!characterData) {
		return;
	}

	AccountData* accountData = accountMgr.Get(characterData->GetOwner());
	if (!accountData) {
		return;
	}

	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		return;
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified character unload detected targeting: uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//pop from the rooms
	roomMgr.PopCharacter(characterData);

	//pump character delete
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = argPacket->characterIndex;
	pumpPacketProximity(static_cast<SerialPacket*>(&newPacket), characterData->GetRoomIndex());

	//unload the character
	characterMgr.Unload(argPacket->characterIndex);
}

//-------------------------
//character movement
//-------------------------

//TODO: (2) Could replace this verbosity with a "verify" method, taking a client, account and character ptr as arguments

void ServerApplication::hCharacterMovement(CharacterPacket* const argPacket) {
	//get the specified objects
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);

	if (!accountData) {
		throw(std::runtime_error("Failed to move a character, missing account"));
	}

	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);

	if (!characterData) {
		throw(std::runtime_error("Failed to move a character, missing character"));
	}

	//get this account's client
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified set character motion targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//check if allowed
	if (characterData->GetOwner() != argPacket->accountIndex && !accountData->GetModerator() && !accountData->GetAdministrator()) {
		//TODO: (2) send to the client?
		std::cerr << "Failed to set character motion due to lack of permissions targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//check if moving rooms
	if (characterData->GetRoomIndex() != argPacket->roomIndex) {
		//delete from the old room
		CharacterPacket newPacket;
		copyCharacterToPacket(&newPacket, argPacket->characterIndex);
		newPacket.type = SerialPacketType::CHARACTER_DELETE;
		pumpPacketProximity(&newPacket, characterData->GetRoomIndex());

		//move the character between rooms
		roomMgr.PopCharacter(characterData);
		characterData->SetRoomIndex(argPacket->roomIndex);
		roomMgr.PushCharacter(characterData);

		//create in the new room
		copyCharacterToPacket(&newPacket, argPacket->characterIndex);
		newPacket.type = SerialPacketType::CHARACTER_CREATE;
		pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
	}
	//if not moving between rooms
	else {
		//set the character's origin and motion
		characterData->SetOrigin(argPacket->origin);
		characterData->SetMotion(argPacket->motion);

		//update the clients
		CharacterPacket newPacket;
		copyCharacterToPacket(&newPacket, argPacket->characterIndex);
		newPacket.type = SerialPacketType::CHARACTER_MOVEMENT;
		pumpPacketProximity(&newPacket, characterData->GetRoomIndex());
	}
}

void ServerApplication::hCharacterAttack(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

void ServerApplication::hCharacterDamage(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}