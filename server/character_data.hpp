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
#ifndef CHARACTERDATA_HPP_
#define CHARACTERDATA_HPP_

//POD members
#include "bbox.hpp"
#include "vector2.hpp"

#include <string>

struct CharacterData {
	//metadata
	int clientIndex;
	std::string username;
	std::string handle;
	std::string avatar;

	//world position
	int mapIndex = 0;
	Vector2 position = {0.0,0.0};
	Vector2 motion = {0.0,0.0};
	BBox bbox = {0,0,0,0};

	//statistics
	int level = 0;
	int exp = 0;
	int maxHP = 0;
	int health = 0;
	int maxMP = 0;
	int mana = 0;
	int attack = 0;
	int defence = 0;
	int intelligence = 0;
	int resistance = 0;
	float accuracy = 0.0;
	float evasion = 0.0;
	float luck = 0.0;

	//uid
	static int uidCounter;
};

#endif
