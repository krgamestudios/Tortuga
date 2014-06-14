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
#ifndef COMBATDATA_HPP_
#define COMBATDATA_HPP_

#include "vector2.hpp"

//gameplay members
#include "character_data.hpp"
#include "enemy_data.hpp"

//graphics
#ifdef GRAPHICS
 #include "sprite_sheet.hpp"
#endif

//std namespace
#include <chrono>
#include <array>
#include <utility>

#define COMBAT_MAX_CHARACTERS 12
#define COMBAT_MAX_ENEMIES 12

struct CombatData {
	enum class Terrain {
		//TODO: types of combat terrains
		NONE = 0,
		GRASSLANDS,
	};

	typedef std::chrono::steady_clock Clock;

	std::array<CharacterData, COMBAT_MAX_CHARACTERS> characterArray;
	std::array<EnemyData, COMBAT_MAX_ENEMIES> enemyArray;

	//world interaction
	int mapIndex = 0;
	Vector2 origin = {0.0,0.0};
	Vector2 bounds = {0.0,0.0};

	//time interval
	Clock::time_point lastTick = Clock::now();

	//graphics
#ifdef GRAPHICS
	SpriteSheet sprite;
#endif
};

#endif
