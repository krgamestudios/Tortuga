#ifndef CLIENTDATA_HPP_
#define CLIENTDATA_HPP_

#include "vector2.hpp"

#include <string>

struct ClientData {
	ClientData() = default;
	ClientData(int ID, int ch, std::string h, std::string a) {
		playerID = ID;
		channel = ch;
		handle = h;
		avatar = a;
	}
	void Update(double delta) {
		position += motion * delta;
	}

	int playerID = -1;
	int channel = -1;
	Vector2 position;
	Vector2 motion;
	std::string handle;
	std::string avatar;
	enum class Command {
		NORMAL,
		CHANGED,
	}command = Command::NORMAL;
};

#endif
