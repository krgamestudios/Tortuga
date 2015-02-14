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
#include "world.hpp"

#include "channels.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

//-------------------------
//character management
//-------------------------

//DOCS: preexisting characters will result in query responses
//DOCS: new characters will result in create messages
//DOCS: this client's character will exist in both (skipped)

void World::hCharacterCreate(CharacterPacket* const argPacket) {
	//prevent double message
	if (characterMap.find(argPacket->characterIndex) != characterMap.end()) {
		std::ostringstream msg;
		msg << "Double character creation event; ";
		msg << "Index: " << argPacket->characterIndex << "; ";
		msg << "Handle: " << argPacket->handle;
		throw(std::runtime_error(msg.str()));
	}

	//implicity create and retrieve the entity
	BaseCharacter* character = &characterMap[argPacket->characterIndex];

	//fill the character's info
	character->SetOrigin(argPacket->origin);
	character->SetMotion(argPacket->motion);
	character->SetBounds({CHARACTER_BOUNDS_X, CHARACTER_BOUNDS_Y, CHARACTER_BOUNDS_WIDTH, CHARACTER_BOUNDS_HEIGHT}); //TODO: (1) send the bounds from the server
	character->SetHandle(argPacket->handle);
	character->SetAvatar(argPacket->avatar);
	character->SetOwner(argPacket->accountIndex);
	character->CorrectSprite();

	//check for this player's character
	if (character->GetOwner() == accountIndex) {
		localCharacter = static_cast<LocalCharacter*>(character);

		//focus the camera on this character
		camera.marginX = (camera.width / 2 - localCharacter->GetSprite()->GetImage()->GetClipW() / 2);
		camera.marginY = (camera.height/ 2 - localCharacter->GetSprite()->GetImage()->GetClipH() / 2);

		//focus on this character's info
		characterIndex = argPacket->characterIndex;
		roomIndex = argPacket->roomIndex;
	}

	//debug
	std::cout << "Character Create, total: " << characterMap.size() << std::endl;
}

void World::hCharacterDelete(CharacterPacket* const argPacket) {
	//ignore if this character doesn't exist
	std::map<int, BaseCharacter>::iterator characterIt = characterMap.find(argPacket->characterIndex);
	if (characterIt == characterMap.end()) {
		return;
	}

	//check for this player's character
	if ((*characterIt).second.GetOwner() == accountIndex) {
		localCharacter = nullptr;

		//clear the camera
		camera.marginX = 0;
		camera.marginY = 0;

		//clear the room
		roomIndex = -1;
	}

	//remove this character
	characterMap.erase(characterIt);

	//debug
	std::cout << "Character Delete, total: " << characterMap.size() << std::endl;
}

void World::hQueryCharacterExists(CharacterPacket* const argPacket) {
	//prevent a double message about this player's character
	if (argPacket->accountIndex == accountIndex) {
		return;
	}

	//ignore characters in a different room (sub-optimal)
	if (argPacket->roomIndex != roomIndex) {
		return;
	}

	//implicitly construct the character if it doesn't exist
	BaseCharacter* character = &characterMap[argPacket->characterIndex];

	//set/update the character's info
	character->SetOrigin(argPacket->origin);
	character->SetMotion(argPacket->motion);
	character->SetBounds({CHARACTER_BOUNDS_X, CHARACTER_BOUNDS_Y, CHARACTER_BOUNDS_WIDTH, CHARACTER_BOUNDS_HEIGHT});
	character->SetHandle(argPacket->handle);
	character->SetAvatar(argPacket->avatar);
	character->SetOwner(argPacket->accountIndex);
	character->CorrectSprite();

	//debug
	std::cout << "Character Query, total: " << characterMap.size() << std::endl;
}

void World::hQueryCharacterStats(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

void World::hQueryCharacterLocation(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

void World::hCharacterMovement(CharacterPacket* const argPacket) {
	//TODO: (1) Authentication
	if (argPacket->characterIndex == characterIndex) {
		return;
	}

	//check that this character exists
	std::map<int, BaseCharacter>::iterator characterIt = characterMap.find(argPacket->characterIndex);
	if (characterIt != characterMap.end()) {
		//set the origin and motion
		characterIt->second.SetOrigin(argPacket->origin);
		characterIt->second.SetMotion(argPacket->motion);
		characterIt->second.CorrectSprite();
	}
}

void World::hCharacterAttack(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

void World::hCharacterDamage(CharacterPacket* const argPacket) {
	//TODO: (9) empty
}

//-------------------------
//player movement & collision
//-------------------------

void World::SendLocalCharacterMovement() {
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_MOVEMENT;

	newPacket.accountIndex = accountIndex;
	newPacket.characterIndex = characterIndex;
	newPacket.roomIndex = roomIndex;
	newPacket.origin = localCharacter->GetOrigin();
	newPacket.motion = localCharacter->GetMotion();

	network.SendTo(Channels::SERVER, &newPacket);
}

std::list<BoundingBox> World::GenerateCollisionGrid(Entity* ptr, int tileWidth, int tileHeight) {
	//prepare for collisions
	BoundingBox wallBounds = {0, 0, tileWidth, tileHeight};
	std::list<BoundingBox> boxList;

	//NOTE: for loops were too dense to work with, so I've just used while loops

	//outer loop
	wallBounds.x = snapToBase((double)wallBounds.w, ptr->GetOrigin().x);
	while(wallBounds.x < (ptr->GetOrigin() + ptr->GetBounds()).x + ptr->GetBounds().w) {
		//inner loop
		wallBounds.y = snapToBase((double)wallBounds.h, ptr->GetOrigin().y);
		while(wallBounds.y < (ptr->GetOrigin() + ptr->GetBounds()).y + ptr->GetBounds().h) {
			//check to see if this tile is solid
			if (regionPager.GetSolid(wallBounds.x / wallBounds.w, wallBounds.y / wallBounds.h)) {
				//push onto the box set
				boxList.push_front(wallBounds);
			}

			//increment
			wallBounds.y += wallBounds.h;
		}

		//increment
		wallBounds.x += wallBounds.w;
	}

	return std::move(boxList);
}