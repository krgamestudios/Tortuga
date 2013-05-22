#include "player_manager.hpp"

#include <stdexcept>

void PlayerManager::UpdateAll(int delta) {
	for (auto it : playerMap) {
		it.second->Update(delta);
	}
}

Player* PlayerManager::New(int playerID, int channel, std::string handle, std::string avatar) {
	if (playerMap.find(playerID) != playerMap.end()) {
		throw(std::runtime_error("Player ID already exists"));
	}
	return playerMap[playerID] = new Player(playerID, channel, handle, avatar);
}

Player* PlayerManager::Get(int playerID) {
	for (auto it : playerMap) {
		if (it.second->GetPlayerID() == playerID) {
			return it.second;
		}
	}
	return nullptr;
}

void PlayerManager::Delete(int playerID) {
	for (auto it : playerMap) {
		if (it.second->GetPlayerID() == playerID) {
			delete it.second;
			playerMap.erase(playerID);
			return;
		}
	}
}

void PlayerManager::DeleteAll() {
	for (auto it : playerMap) {
		delete it.second;
	}
	playerMap.clear();
}
