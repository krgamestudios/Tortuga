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

//components
#include "character_defines.hpp"
#include "vector2.hpp"
#include "statistics.hpp"

//std namespace
#include <string>
#include <cmath>

class CharacterData {
public:
	CharacterData();
	~CharacterData();

	//location and movement
	int SetRoomIndex(int i) { return roomIndex = i; }
	Vector2 SetOrigin(Vector2 v) { return origin = v; }
	Vector2 SetMotion(Vector2 v) { return motion = v; }

	int GetRoomIndex() { return roomIndex; }
	Vector2 GetOrigin() { return origin; }
	Vector2 GetMotion() { return motion; }

	//accessors and mutators
	Statistics* GetBaseStats() { return &baseStats; }

	//database stuff
	int GetOwner() { return owner; }
	std::string GetHandle() { return handle; }
	std::string GetAvatar() { return avatar; }

private:
	friend class CharacterManager;

	//world position
	int roomIndex = 0;
	Vector2 origin = {0.0,0.0};
	Vector2 motion = {0.0,0.0};

	Statistics baseStats;

	int owner;
	std::string handle;
	std::string avatar;
};

#endif
