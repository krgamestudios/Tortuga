#include "player.hpp"

Player::Player(int ID, int ch, std::string h, std::string a) {
	playerID = ID;
	channel = ch;
	handle = h;
	avatar = a;
}

void Player::Update(int delta) {
	position += motion * delta;
}