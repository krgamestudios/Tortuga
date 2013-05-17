#ifndef PLAYERMANAGER_H_
#define PLAYERMANAGER_H_

#include "player.hpp"

#include <map>
#include <string>

class PlayerManager {
public:
	PlayerManager() = default;
	~PlayerManager() = default;
private:
	//utilities
	typedef std::map<std::string, Player*> PlayerMap;

	//members
	PlayerMap playerMap;
	int maxPlayers;
	int ticker;
};

#endif
