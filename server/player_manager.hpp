#ifndef PLAYERMANAGER_H_
#define PLAYERMANAGER_H_

#include "player.hpp"

#include <list>
#include <string>

class PlayerManager {
private:
	//utilities
	typedef std::list<Player*> PlayerList;
public:
	PlayerManager() = default;
	PlayerManager(int maxPlayers);
	~PlayerManager();

	void UpdateAll(int delta);

	Player* New(int playerID, int channel, std::string handle, std::string avatar);
	Player* Get(int playerID);
	void Delete(int playerID);

	void DeleteAll();

	PlayerList* GetPlayerList() {
		return &playerList;
	}
private:
	//utilities
	//...

	//members
	PlayerList playerList;
	int maxPlayers = 0;
	int ticker = 0;
};

#endif
