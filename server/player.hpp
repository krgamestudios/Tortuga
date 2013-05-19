#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "vector2.hpp"

#include <string>

class Player {
public:
	Player() = default;
	Player(int playerID, int channel, std::string handle, std::string avatar);
	void Update(int delta);

	int SetPlayerID(int id) {return playerID = id;}
	int GetPlayerID() const {return playerID;}
	int SetChannel(int ch) {return channel = ch;}
	int GetChannel() const {return channel;}

	Vector2 SetPosition(Vector2 v) {return position = v;}
	Vector2 ShiftPosition(Vector2 v) {return position += v;}
	Vector2 GetPosition() const {return position;}
	Vector2 SetMotion(Vector2 v) {return motion = v;}
	Vector2 ShiftMotion(Vector2 v) {return motion += v;}
	Vector2 GetMotion() const {return motion;}

	std::string SetAvatar(std::string s) {return avatar = s;}
	std::string GetAvatar() const {return avatar;}
	std::string SetHandle(std::string s) {return handle = s;}
	std::string GetHandle() const {return handle;}
private:
	int playerID = -1;
	int channel = -1; //for networking
	Vector2 position;
	Vector2 motion;
	std::string avatar;
	std::string handle;
};

#endif
