#ifndef PLAYERMANAGER_HPP_
#define PLAYERMANAGER_HPP_

#include <list>

class PlayerManager {
private:
	PlayerManager() = default;
	~PlayerManager() = default;
	static PlayerManager instance;

public:
	static PlayerManager* GetInstance() { return &instance; }

private:
	struct PlayerEntry {
		int index;
	};

	std::list<PlayerEntry> list;
};

#endif
