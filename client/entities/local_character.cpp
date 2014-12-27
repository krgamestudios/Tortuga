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

void LocalCharacter::ProcessCollisions(std::list<BoundingBox>& boxList) {
	if (CheckCollisionSimple(boxList, origin + motion)) {
		Vector2 velocity;
		velocity.x = CorrectVelocityX(boxList, motion.x);
		velocity.y = CorrectVelocityY(boxList, motion.y);
		origin += velocity;
	}
	else {
		origin += motion;
	}
}

bool LocalCharacter::CheckCollisionSimple(std::list<BoundingBox>& boxList, Vector2 newPos) {
	for (auto& it : boxList) {
		if (it.CheckOverlap(bounds + newPos)) {
			return true;
		}
	}
	return false;
}

double LocalCharacter::CorrectVelocityX(std::list<BoundingBox>& boxList, double velocityX) {
	double ret = velocityX;
	for (auto& it : boxList) {
		if (it.CheckOverlap(bounds + origin + Vector2(velocityX, 0) )) {
			if (velocityX > 0) {
				ret = std::min(ret, it.x - origin.x - (bounds.w - bounds.x - 1));
			}
			else if (velocityX < 0) {
				ret = std::max(ret, (it.x + it.w) - origin.x);
			}
		}
	}
	return ret;
}

double LocalCharacter::CorrectVelocityY(std::list<BoundingBox>& boxList, double velocityY) {
	return velocityY;
}
