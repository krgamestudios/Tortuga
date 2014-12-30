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

void ServerApplication::HandleCharacterCreate(CharacterPacket* const argPacket) {
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

	//pump this character to all clients
	CharacterPacket newPacket;
	CopyCharacterToPacket(&newPacket, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	PumpPacket(&newPacket);
}

void ServerApplication::HandleCharacterDelete(CharacterPacket* const argPacket) {
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

	//delete the character
	characterMgr.Delete(characterIndex);

	//pump character delete
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = characterIndex;
	PumpPacket(static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::HandleCharacterLoad(CharacterPacket* const argPacket) {
	int characterIndex = characterMgr.Load(argPacket->accountIndex, argPacket->handle, argPacket->avatar);

	if (characterIndex < 0) {
		//build the error message
		std::ostringstream msg;
		if (characterIndex == -1) {
			msg << "Character already loaded: ";
		}
		if (characterIndex == -1) {
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

	//pump this character to all clients
	CharacterPacket newPacket;
	CopyCharacterToPacket(&newPacket, characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_CREATE;
	PumpPacket(&newPacket);
}

void ServerApplication::HandleCharacterUnload(CharacterPacket* const argPacket) {
	//get the entries
	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);
	if (!characterData) {
		return;
	}

	AccountData* accountData = accountMgr.Get(characterData->GetOwner());
	if (!accountData) {
		return; //TODO: logic_error
	}

	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		return; //TODO: logic_error
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified character unload detected targeting: uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//unload the character
	characterMgr.Unload(argPacket->characterIndex);

	//pump character delete
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_DELETE;
	newPacket.characterIndex = argPacket->characterIndex;
	PumpPacket(static_cast<SerialPacket*>(&newPacket));
}

//-------------------------
//character movement
//-------------------------

//TODO: ? Could replace this verbosity with a "verify" method, taking a client, account and character ptr as arguments

void ServerApplication::HandleCharacterSetRoom(CharacterPacket* const argPacket) {
	//get the specified objects
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);

	if (!accountData || !characterData) {
		throw(std::runtime_error("Failed to set character room, missing data"));
	}

	//get this account's client
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified set character origin targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//check if allowed
	if (characterData->GetOwner() != argPacket->accountIndex && !accountData->GetModerator() && !accountData->GetAdministrator()) {
		//TODO: send to the client?
		std::cerr << "Failed to set character room due to lack of permissions targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//set the character's room, zero it's origin, zero it's motion
	characterData->SetRoomIndex(argPacket->roomIndex);
	characterData->SetOrigin({0, 0});
	characterData->SetMotion({0, 0});

	//update the clients
	CharacterPacket newPacket;
	CopyCharacterToPacket(&newPacket, argPacket->characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_SET_ROOM;
	PumpPacket(&newPacket);
}

void ServerApplication::HandleCharacterSetOrigin(CharacterPacket* const argPacket) {
	//get the specified objects
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);

	if (!accountData || !characterData) {
		throw(std::runtime_error("Failed to set character origin, missing data"));
	}

	//get this account's client
	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified set character origin targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//check if allowed
	if (characterData->GetOwner() != argPacket->accountIndex && !accountData->GetModerator() && !accountData->GetAdministrator()) {
		//TODO: send to the client?
		std::cerr << "Failed to set character origin due to lack of permissions targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//set the character's origin, zero it's motion
	characterData->SetOrigin(argPacket->origin);
	characterData->SetMotion({0, 0});

	//update the clients
	CharacterPacket newPacket;
	CopyCharacterToPacket(&newPacket, argPacket->characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_SET_ORIGIN;
	PumpPacket(&newPacket);
}

void ServerApplication::HandleCharacterSetMotion(CharacterPacket* const argPacket) {
	//get the specified objects
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);

	if (!accountData) {
		std::ostringstream msg;
		msg << "Failed to set character motion, missing account: Index " << argPacket->accountIndex << "; ";
		msg << "Number of accounts loaded: " << accountMgr.GetContainer()->size();
		throw(std::runtime_error(msg.str()));
	}

	CharacterData* characterData = characterMgr.Get(argPacket->characterIndex);

	if (!characterData) {
		std::ostringstream msg;
		msg << "Failed to set character motion, missing character: Index " << argPacket->characterIndex << "; ";
		msg << "Number of characters loaded: " << characterMgr.GetContainer()->size();
		throw(std::runtime_error(msg.str()));
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
		//TODO: send to the client?
		std::cerr << "Failed to set character motion due to lack of permissions targeting uid(" << argPacket->characterIndex << ")" << std::endl;
		return;
	}

	//set the character's origin and motion
	characterData->SetOrigin(argPacket->origin);
	characterData->SetMotion(argPacket->motion);

	//update the clients
	CharacterPacket newPacket;
	CopyCharacterToPacket(&newPacket, argPacket->characterIndex);
	newPacket.type = SerialPacketType::CHARACTER_SET_MOTION;
	PumpPacket(&newPacket);
}
