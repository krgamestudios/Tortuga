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
#ifndef ENEMYFACTORYINTERFACE_HPP_
#define ENEMYFACTORYINTERFACE_HPP_

#include "enemy_data.hpp"
#include "map_type.hpp"

#include <list>

//NOTE: Based on biome, world difficulty, etc.
class EnemyFactoryInterface {
public:
	EnemyFactoryInterface() = default;
	virtual ~EnemyFactoryInterface() = default;

	virtual void Generate(std::list<EnemyData>* container) = 0;

	//control the difficulty of the room
	MapType SetType(MapType t) { return type = t; }
	MapType GetType() { return type; }

	int SetDifficulty(int d) { return difficulty = d; }
	int GetDifficulty() { return difficulty; }
protected:
	MapType type;
	int difficulty = 0;
};

#endif
