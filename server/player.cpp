#include "player.hpp"

Player::Player(int id) : clientID(id) {
	//
}

void Player::Update(int delta) {
	position += motion * delta;
}