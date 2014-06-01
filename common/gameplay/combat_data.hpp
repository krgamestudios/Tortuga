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

//POD members
#include "vector2.hpp"
#include "bbox.hpp"

//gameplay members
#include "character_data.hpp"
#include "enemy_data.hpp"

//graphics
#ifdef GRAPHICS
 #include "sprite_sheet.hpp"
#endif

//std namespace
#include <chrono>
#include <list>
#include <utility>

#define COMBAT_MAX_CHARACTER_COUNT 12
#define COMBAT_MAX_ENEMY_COUNT 12

struct CombatData {
	enum class Terrain {
		//TODO: types of terrains
		NONE = 0,
		GRASSLANDS,
	};

	typedef std::chrono::steady_clock Clock;

	//combatants, point to the std::map's internal pairs
	std::list<std::pair<const int, CharacterData>*> characterList;
	std::list<std::pair<const int, EnemyData>*> enemyList;

	//world interaction
	int mapIndex = 0;
	Vector2 position = {0.0,0.0};
	BBox bbox = {0,0,0,0};

	//time interval
	Clock::time_point lastTick = Clock::now();

	//graphics
#ifdef GRAPHICS
	SpriteSheet sprite;
#endif
};

#endif
