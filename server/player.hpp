#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "vector2.hpp"

#include <string>

//TODO

class Player {
public:
	Player();
	~Player();

	void Update(int);

	Vector2 GetPosition();
private:
	int clientID;
	Vector2 position;
	Vector2 motion;
	std::string avatarName;
};

#endif
