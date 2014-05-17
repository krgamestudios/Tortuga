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

#include "utility.hpp"

int ServerApplication::CreateCombatInstance(int mapIndex, int x, int y) {
	CombatData& combat = combatMap[CombatData::uidCounter];

	combat.mapIndex = mapIndex;
	combat.position.x = x;
	combat.position.y = y;

	//explicitly postfix
	return CombatData::uidCounter++;
}

void ServerApplication::UnloadCombatInstance(int uid) {
	for (auto& it : combatMap[uid].characterList) {
		it->inCombat = false;
	}
	combatMap.erase(uid);
}

void ServerApplication::UpdateCombat() {
	for (auto& combat : combatMap) {
		//prune characters that have left
		erase_if(combat.second.characterList, [](CharacterData* it) -> bool {
			return !it->inCombat;
		});

		//TODO: prune dead enemies

		//update the instance once per second
		if (CombatData::Clock::now() - combat.second.lastTick > std::chrono::duration<int>(1)) {
			//increase the ATB gauges
			for (auto& it : combat.second.characterList) {
				it->atbGauge += it->speed;
			}
			for (auto& it : combat.second.enemyList) {
				it.atbGauge += it.speed;
			}
			combat.second.lastTick = CombatData::Clock::now();

			//execute the combat commands
			for (auto& it : combat.second.characterList) {
				if (it->atbGauge >= 100 && /* TODO: Check that there is something stored... */ true ) {
					it->atbGauge = 0;
					//TODO: EXECUTE STORED COMMAND
				}
			}
			for (auto& it : combat.second.enemyList) {
				if (it.atbGauge >= 100) {
					it.atbGauge = 0;
					//TODO: EXECUTE AI SCRIPT
				}
			}
		}
	}

	//Erase instances with no enemies left
	erase_if(combatMap, [](std::pair<const int, CombatData>& combat) -> bool {
		if (combat.second.enemyList.size() == 0) {
			//kick the characters out
			for (auto& it : combat.second.characterList) {
				it->inCombat = false;
			}
			return true;
		}
		return false;
	});

	//TODO: reset instances with no players?
}

void ServerApplication::PushCharacterToCombat() {
	//TODO
}

void ServerApplication::PopCharacterFromCombat() {
	//TODO
}