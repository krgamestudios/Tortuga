#ifndef COMBATROOM_HPP_
#define COMBATROOM_HPP_

#include "base_room.hpp"

class CombatRoom : public BaseRoom {
public:
	CombatRoom();
	~CombatRoom();

	void Init();
	void Loop();
	void Quit();

private:
	//parent room index
	//combatants
	//monsters
};

#endif