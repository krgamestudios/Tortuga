#ifndef PLAYERMANAGER_H_
#define PLAYERMANAGER_H_

#include "player.hpp"

#include <map>
#include <string>

class PlayerManager {
private:
	//utilities
	typedef std::map<int, Player*> PlayerMap;
public:
	PlayerManager() = default;
	PlayerManager(int max) {SetMaxPlayers(max);}
	~PlayerManager() {DeleteAll();}

	void UpdateAll(int delta);

	Player* New(int playerID, int channel, std::string handle, std::string avatar);
	Player* Get(int playerID);
	void Delete(int playerID);

	void DeleteAll();

	PlayerMap* GetPlayerMap() {return &playerMap;}
	int SetMaxPlayers(int max) {return maxPlayers = max;}
	int GetMaxPlayers() const {return maxPlayers;}
private:
	//utilities
	//...

	//members
	PlayerMap playerMap;
	int maxPlayers = 0;
};

#endif
