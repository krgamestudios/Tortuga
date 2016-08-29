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
#pragma once

#include "character_data.hpp"
#include "creature_data.hpp"

#include <functional>

class BattleData {
public:
	constexpr static int BATTLE_SIZE = 8;

	BattleData();
	~BattleData();

	void Update();

	//accessors and mutators
	int PushCharacter(CharacterData* const characterData);
	int PopCharacter(CharacterData const * const characterData);

	int PushCreature(CreatureData* const creatureData);
	int PopCreature(CreatureData const * const creatureData);

private:
	std::array<CharacterData*, BATTLE_SIZE> characterArray;
	std::array<CreatureData*, BATTLE_SIZE> creatureArray;
};