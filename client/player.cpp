#include "player.hpp"

#define ANIMATION_SPEED 0.1
#define WALKING_SPEED 140

Player::Player(SDL_Surface* s, int w, int h)
	: sprite(s, w, h)
{
}

void Player::Update(double delta) {
	if (motion.y > 0) {
		FaceDirection(Direction::SOUTH);
	}
	else if (motion.y < 0) {
		FaceDirection(Direction::NORTH);
	}
	else if (motion.x < 0) {
		FaceDirection(Direction::WEST);
	}
	else if (motion.x > 0) {
		FaceDirection(Direction::EAST);
	}

	if (motion.x != 0 && motion.y != 0) {
		sprite.SetInterval(ANIMATION_SPEED);
		constexpr double d = 1/sqrt(2);
		position += motion * d * delta;
	}
	else if (motion != 0) {
		sprite.SetInterval(ANIMATION_SPEED);
		position += motion * delta;
	}
	else {
		sprite.SetCurrentFrame(0);
		sprite.SetInterval(0);
	}
	sprite.Update(delta);
}

void Player::WalkInDirection(Direction d) {
	switch(d) {
		case Direction::NORTH:
			if (motion.y >= 0) {
				motion.y -= WALKING_SPEED;
			}
			else {
				motion.y = -WALKING_SPEED;
			}
		break;
		case Direction::SOUTH:
			if (motion.y <= 0) {
				motion.y += WALKING_SPEED;
			}
			else {
				motion.y = WALKING_SPEED;
			}
		break;
		case Direction::WEST:
			if (motion.x >= 0) {
				motion.x -= WALKING_SPEED;
			}
			else {
				motion.x = -WALKING_SPEED;
			}
		break;
		case Direction::EAST:
			if (motion.x <= 0) {
				motion.x += WALKING_SPEED;
			}
			else {
				motion.x = WALKING_SPEED;
			}
		break;
	}
}

void Player::FaceDirection(Direction dir) {
	switch(dir) {
		case Direction::SOUTH:
			sprite.SetCurrentStrip(0);
		break;
		case Direction::NORTH:
			sprite.SetCurrentStrip(1);
		break;
		case Direction::WEST:
			sprite.SetCurrentStrip(2);
		break;
		case Direction::EAST:
			sprite.SetCurrentStrip(3);
		break;
	}
}