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

#define WALKING_SPEED 140

void PlayerCharacter::Update(double delta) {
	if (diagonal) {
		constexpr double d = 1.0/sqrt(2);
		position += motion * delta * d;
	}
	else {
		position += motion * delta;
	}
	sprite.Update(delta);
}

void PlayerCharacter::AdjustDirection(Direction direction) {
	//shift the movement in this direction
	switch(direction) {
		case Direction::NORTH:
			if (motion.y >= 0) {
				motion.y -= WALKING_SPEED;
			}
		break;
		case Direction::SOUTH:
			if (motion.y <= 0) {
				motion.y += WALKING_SPEED;
			}
		break;
		case Direction::WEST:
			if (motion.x >= 0) {
				motion.x -= WALKING_SPEED;
			}
		break;
		case Direction::EAST:
			if (motion.x <= 0) {
				motion.x += WALKING_SPEED;
			}
		break;
	}
	//face the correct direction
	ResetDirection();
}

void PlayerCharacter::FaceDirection(Direction direction) {
	//this function depends on the format of the sprite sheets
	switch(direction) {
		case Direction::NORTH:
			sprite.SetYIndex(1);
		break;
		case Direction::SOUTH:
			sprite.SetYIndex(0);
		break;
		case Direction::WEST:
			sprite.SetYIndex(2);
		break;
		case Direction::EAST:
			sprite.SetYIndex(3);
		break;
	}
}

void PlayerCharacter::ResetDirection() {
	//base the direction on the character's movement
	if (motion.y > 0) {
		FaceDirection(Direction::SOUTH);
	}
	else if (motion.y < 0) {
		FaceDirection(Direction::NORTH);
	}
	else if (motion.x > 0) {
		FaceDirection(Direction::EAST);
	}
	else if (motion.x < 0) {
		FaceDirection(Direction::WEST);
	}
	ResetSpeed();
}

void PlayerCharacter::ResetSpeed() {
	//diagonal
	if (motion.x != 0 && motion.y != 0) {
		sprite.SetDelay(0.1);
		diagonal = true;
	}
	//cardinal
	else if (motion != 0) {
		sprite.SetDelay(0.1);
		diagonal = false;
	}
	//not moving
	else {
		sprite.SetDelay(0);
		sprite.SetXIndex(0);
		diagonal = false;
	}
}
