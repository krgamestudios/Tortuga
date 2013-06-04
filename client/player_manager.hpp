#ifndef PLAYERMANAGER_HPP_
#define PLAYERMANAGER_HPP_

#include "player.hpp"

#include <map>

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager();

	Player* New(int index, SDL_Surface* avatarSheet);
	Player* Get(int index);
	void Delete(int index);

	void UpdateAll(double delta);
	void DrawAllTo(SDL_Surface* dest);
	void DeleteAll();

	Player* operator[](int i) { return Get(i); }
private:
	std::map<int, Player*> playerMap;
};

#endif
