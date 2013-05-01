#include "player_manager.hpp"

#include <stdexcept>

PlayerManager::PlayerManager() {
	//
}

PlayerManager::~PlayerManager() {
	DeleteAll();
}

Player* PlayerManager::New(int index, SDL_Surface* avatarSheet) {
	if (playerMap.find(index) != playerMap.end()) {
		throw(std::runtime_error("This player's index is already taken"));
	}

	return playerMap[index] = new Player(avatarSheet, avatarSheet->w/4, avatarSheet->h/4);
}

Player* PlayerManager::Get(int index) {
	std::map<int, Player*>::iterator it = playerMap.find(index);
	if(it == playerMap.end()) {
		throw(std::runtime_error("Failed to find a player with that index"));
	}

	return it->second;
}

void PlayerManager::Delete(int index) {
	std::map<int, Player*>::iterator it = playerMap.find(index);
	delete it->second;
	playerMap.erase(it);
}

void PlayerManager::UpdateAll(int delta) {
	for (auto it : playerMap) {
		it.second->Update(delta);
	}
}

void PlayerManager::DrawAllTo(SDL_Surface* dest) {
	for (auto it : playerMap) {
		it.second->DrawTo(dest);
	}
}

void PlayerManager::DeleteAll() {
	for (auto it : playerMap) {
		delete it.second;
	}
	playerMap.clear();
}
