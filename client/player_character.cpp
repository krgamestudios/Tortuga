/* Copyright: (c) Kayne Ruse 2013
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
#include "player_character.hpp"

void PlayerCharacter::Update(double delta) {
	if (limitSpeed) {
		constexpr double d = 1.0/sqrt(2);
		position += motion * delta * d;
	}
	else {
		position += motion * delta;
	}
	sprite.Update(delta);
}

void PlayerCharacter::MoveDirection(CardinalDirection cd) {
	//shift the movement in this direction
	switch(cd) {
		case CardinalDirection::NORTH:
			if (motion.y >= 0) {
				motion.y -= WALKING_SPEED;
			}
		break;
		case CardinalDirection::SOUTH:
			if (motion.y <= 0) {
				motion.y += WALKING_SPEED;
			}
		break;
		case CardinalDirection::EAST:
			if (motion.x >= 0) {
				motion.x -= WALKING_SPEED;
			}
		break;
		case CardinalDirection::WEST:
			if (motion.x <= 0) {
				motion.x += WALKING_SPEED;
			}
		break;
	}
	//short cut
	if (motion.x != 0 && motion.y != 0) {
		sprite.SetInterval(0.1);
		limitSpeed = true;
	}
	else if (motion.x != 0 || motion.y != 0) {
		sprite.SetInterval(0.1);
		limitSpeed = false;
	}
	else {
		sprite.SetInterval(0);
		sprite.SetCurrentFrame(0);
		limitSpeed = false;
	}
	//face the correct direction
	FaceDirection();
}

void PlayerCharacter::FaceDirection(CardinalDirection cd) {
	switch(cd) {
		case CardinalDirection::NORTH:
			sprite.SetCurrentStrip(1);
		break;
		case CardinalDirection::SOUTH:
			sprite.SetCurrentStrip(0);
		break;
		case CardinalDirection::EAST:
			sprite.SetCurrentStrip(2);
		break;
		case CardinalDirection::WEST:
			sprite.SetCurrentStrip(3);
		break;
	}
}

void PlayerCharacter::FaceDirection() {
	//base the direction on the character's movement
	if (motion.y > 0) {
		FaceDirection(CardinalDirection::SOUTH);
	}
	else if (motion.y < 0) {
		FaceDirection(CardinalDirection::NORTH);
	}
	else if (motion.x < 0) {
		FaceDirection(CardinalDirection::EAST);
	}
	else if (motion.x > 0) {
		FaceDirection(CardinalDirection::WEST);
	}
}
