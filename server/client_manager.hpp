#ifndef CLIENTMANAGER_HPP_
#define CLIENTMANAGER_HPP_

#include <list>

class ClientManager {
private:
	ClientManager() = default;
	~ClientManager() = default;
	static ClientManager instance;

public:
	static ClientManager* GetInstance() { return &instance; }

private:
	struct ClientEntry {
		int index;
	};

	std::list<ClientEntry> list;
};

#endif
