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
#ifndef CHARACTERDATA_HPP_
#define CHARACTERDATA_HPP_

//components
#include "character_defines.hpp"
#include "entity.hpp"

//std namespace
#include <string>
#include <cmath>

#include <iostream>

class CharacterData: public Entity {
public:
	CharacterData() = default;
	CharacterData(CharacterData const& rhs) {
		std::cerr << "Character copy detected" << std::endl;
		owner = rhs.owner;
		handle = rhs.handle;
		avatar = rhs.avatar;

		//entity stuff
		roomIndex = rhs.roomIndex;
		origin = rhs.origin;
		motion = rhs.motion;
	}
	CharacterData(CharacterData&& rhs) {
		std::cerr << "Character move detected" << std::endl;
		owner = rhs.owner;
		handle = rhs.handle;
		avatar = rhs.avatar;

		//entity stuff
		roomIndex = rhs.roomIndex;
		origin = rhs.origin;
		motion = rhs.motion;
	}
	~CharacterData() = default;

	//accessors and mutators
	//...

	//database stuff
	int GetOwner();
	std::string GetHandle();
	std::string GetAvatar();

private:
	friend class CharacterManager;

	int owner;
	std::string handle;
	std::string avatar;
};

#endif
