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
#include "local_character.hpp"

#include <iostream>

bool LocalCharacter::ProcessCollisionGrid(std::list<BoundingBox> boxList, Uint8* keyState) {
	//skip this if there's no movement
	if (motion == 0) {
		return false;
	}

	//determine the simple movement based on input
	Vector2 newMotion = {0, 0};
	if (keyState[SDLK_w]) {
		newMotion.y -= CHARACTER_WALKING_SPEED;
	}
	if (keyState[SDLK_a]) {
		newMotion.x -= CHARACTER_WALKING_SPEED;
	}
	if (keyState[SDLK_s]) {
		newMotion.y += CHARACTER_WALKING_SPEED;
	}
	if (keyState[SDLK_d]) {
		newMotion.x += CHARACTER_WALKING_SPEED;
	}

	bool ret = false;

	for(auto& box : boxList) {
		if (box.CheckOverlap(origin + bounds)) {
			//push the character to the closest non-contact position
			//TODO

			//set any motion in that direction to zero
			//TODO
			ret = true;
		}
	}

	//handle diagonals
	if (newMotion.x != 0 && newMotion.y != 0) {
		newMotion *= CHARACTER_WALKING_MOD;
	}

	//set the new motion
	motion = newMotion;

	//signal for updates
	return ret;
}