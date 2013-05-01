#include "surface_manager.hpp"

#include <stdexcept>

SurfaceManager::SurfaceManager() {
	//
}

SurfaceManager::~SurfaceManager() {
	FreeAll();
}

SDL_Surface* SurfaceManager::Load(std::string key, std::string fname) {
	mapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		throw(std::runtime_error(std::string("Surface already loaded: ") + key + std::string(", ") + fname));
	}
	return LoadSurface(key, fname);
}

SDL_Surface* SurfaceManager::Reload(std::string key, std::string fname) {
	mapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		SDL_FreeSurface(it->second);
		surfaceMap.erase(it);
	}
	return LoadSurface(key, fname);
}

SDL_Surface* SurfaceManager::Get(std::string key) {
	mapType::iterator it = surfaceMap.find(key);
	if (it == surfaceMap.end()) {
		throw(std::runtime_error(std::string("Could not find key: ") + key));
	}
	return it->second;
}

SDL_Surface* SurfaceManager::Set(std::string key, SDL_Surface* ptr) {
	mapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		throw(std::runtime_error(std::string("Key already exists: ") + key));
	}
}

void SurfaceManager::Free(std::string key) {
	mapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		SDL_FreeSurface(it->second);
		surfaceMap.erase(it);
	}
}

void SurfaceManager::FreeAll() {
	for (auto it : surfaceMap) {
		SDL_FreeSurface(it.second);
	}
	surfaceMap.clear();
}

SDL_Surface* SurfaceManager::LoadSurface(std::string key, std::string fname) {
	SDL_Surface* ptr = SDL_LoadBMP(fname.c_str());
	if (ptr == nullptr) {
		throw(std::runtime_error(std::string("Failed to load file: ") + fname));
	}
	SDL_SetColorKey(ptr, SDL_SRCCOLORKEY, SDL_MapRGB(ptr->format, 255, 0, 255)); //default
	return surfaceMap[key] = ptr;
}
