#include "player_manager.hpp"

PlayerManager::PlayerManager(int max) {
	maxPlayers = max;
}

PlayerManager::~PlayerManager() {
	DeleteAll();
}

void PlayerManager::UpdateAll(int delta) {
	for (auto it : playerList) {
		it->Update(delta);
	}
}

Player* PlayerManager::New(int playerID, int channel, std::string handle, std::string avatar) {
	Player* p = new Player(playerID, channel, handle, avatar);
	playerList.push_back(p);
	return p;
}

Player* PlayerManager::Get(int playerID) {
	for (auto it : playerList) {
		if (it->GetPlayerID() == playerID) {
			return it;
		}
	}
	return nullptr;
}

void PlayerManager::Delete(int playerID) {
	for (auto it : playerList) {
		if (it->GetPlayerID() == playerID) {
			delete it;
			playerList.remove(it);
			return;
		}
	}
}

void PlayerManager::DeleteAll() {
	for (auto it : playerList) {
		delete it;
	}
	playerList.clear();
}
