/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#include "surface_manager.hpp"

#include <stdexcept>

SDL_Surface* SurfaceManager::Load(std::string key, std::string fname) {
	MapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		throw(std::runtime_error(std::string("Surface already loaded: ") + key + std::string(", ") + fname));
	}
	return LoadSurface(key, fname);
}

SDL_Surface* SurfaceManager::Reload(std::string key, std::string fname) {
	MapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		SDL_FreeSurface(it->second);
		surfaceMap.erase(it);
	}
	return LoadSurface(key, fname);
}

SDL_Surface* SurfaceManager::Get(std::string key) {
	MapType::iterator it = surfaceMap.find(key);
	if (it == surfaceMap.end()) {
		throw(std::runtime_error(std::string("Could not find key: ") + key));
	}
	return it->second;
}

SDL_Surface* SurfaceManager::Set(std::string key, SDL_Surface* ptr) {
	MapType::iterator it = surfaceMap.find(key);
	if (it != surfaceMap.end()) {
		throw(std::runtime_error(std::string("Key already exists: ") + key));
	}
	return surfaceMap[key] = ptr;
}

void SurfaceManager::Free(std::string key) {
	MapType::iterator it = surfaceMap.find(key);
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
	if (!ptr) {
		throw(std::runtime_error(std::string("Failed to load file: ") + fname));
	}
	SDL_SetColorKey(ptr, SDL_SRCCOLORKEY, SDL_MapRGB(ptr->format, 255, 0, 255)); //default
	return surfaceMap[key] = ptr;
}
