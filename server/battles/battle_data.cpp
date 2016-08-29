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
#include "battle_data.hpp"

#include <algorithm>

BattleData::BattleData() {
	for (int i = 0; i < BATTLE_SIZE; i++) {
		characterArray[i] = nullptr;
		creatureArray[i] = nullptr;
	}
}

BattleData::~BattleData() {
	for (int i = 0; i < BATTLE_SIZE; i++) {
		if (characterArray[i] != nullptr || creatureArray[i] != nullptr) {
			//breaking a cardinal sin
			throw(std::runtime_error("BattleData not empty on destruction"));
		}
	}
}

void BattleData::Update() {
	//TODO: (0) EMPTY
}

//accessors and mutators
int BattleData::PushCharacter(CharacterData* const characterData) {
	//push the character into the battle object
	for (int i = 0; i < BATTLE_SIZE; i++) {
		if (characterArray[i] == nullptr) {
			characterArray[i] = characterData;
			return 1;
		}
	}
	return 0;
}

int BattleData::PopCharacter(CharacterData const * const characterData) {
	//pop the character from the battle object
	for (int i = 0; i < BATTLE_SIZE; i++) {
		if (characterArray[i] == characterData) {
			characterArray[i] = nullptr;
			return 1;
		}
	}
	return 0;
}

int BattleData::PushCreature(CreatureData* const creatureData) {
	//TODO: (0) EMPTY
}

int BattleData::PopCreature(CreatureData const * const creatureData) {
	//TODO: (0) EMPTY
}
