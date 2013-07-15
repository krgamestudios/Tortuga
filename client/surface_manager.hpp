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
#ifndef SURFACEMANAGER_HPP_
#define SURFACEMANAGER_HPP_

#include "SDL/SDL.h"

#include <map>
#include <string>

class SurfaceManager {
public:
	SurfaceManager() = default;
	~SurfaceManager() noexcept { FreeAll(); }

	SDL_Surface* Load(std::string key, std::string fname);
	SDL_Surface* Reload(std::string key, std::string fname);
	SDL_Surface* Get(std::string key);
	SDL_Surface* Set(std::string key, SDL_Surface* ptr);
	void Free(std::string key);
	void FreeAll();

	SDL_Surface* operator[](std::string key) { return Get(key); };
private:
	SDL_Surface* LoadSurface(std::string key, std::string fname);
	typedef std::map<std::string, SDL_Surface*> MapType;
	MapType surfaceMap;
};

#endif
