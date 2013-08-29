#ifndef WORLDROOM_HPP_
#define WORLDROOM_HPP_

#include "base_room.hpp"

class WorldRoom : public BaseRoom {
public:
	WorldRoom();
	~WorldRoom();

	void Init();
	void Loop();
	void Quit();

private:
	//collision map
	//map metadata
	//player list
	//combat portals
};

#endif