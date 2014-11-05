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
#ifndef DOORDATA_HPP_
#define DOORDATA_HPP_

#include "entity.hpp"
#include "vector2.hpp"

#include <string>

class DoorData: public Entity {
public:
	DoorData() = default;
	~DoorData() = default;

	//accessors & mutators
	std::string SetRoomName(std::string);
	Vector2 SetDestPosition(Vector2);

	std::string GetRoomName();
	Vector2 GetDestPosition();

private:
	friend class DoorManager;

	std::string roomName;
	Vector2 destPosition;
};

#endif