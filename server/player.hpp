#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "vector2.hpp"

#include <string>

class Player {
public:
	Player(int id);
	~Player() = default;

	void Update(int);

	int GetClientID() const {
		return clientID;
	}

	Vector2 SetPosition(Vector2 v) {
		return position = v;
	}
	Vector2 GetPosition() const {
		return position;
	}
	Vector2 SetMotion(Vector2 v) {
		return motion = v;
	}
	Vector2 GetMotion() const {
		return motion;
	}
	std::string SetAvatarName(std::string s) {
		return avatarName = s;
	}
	std::string GetAvatarName() {
		return avatarName;
	}
private:
	const int clientID;
	Vector2 position;
	Vector2 motion;
	std::string avatarName;
};

#endif
