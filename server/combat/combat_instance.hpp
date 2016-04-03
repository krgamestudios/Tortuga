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

class CombatInstance {
public:
	CombatInstance();
	~CombatInstance();

	void Update();

	//accessors and mutators
	void PushCharacter(CharacterData* const characterData);
	void PopCharacter(CharacterData* const characterData);

	void PushCreature(CreatureData* const creatureData);
	void PopCreature(CreatureData* const creatureData);

private:
	std::array<CharacterData*, 8> characterArray;
	std::array<CreatureData*, 8> creatureArray;
};