/* Copyright: (c) Kayne Ruse 2013-2015
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
#ifndef CHARACTERMANAGER_HPP_
#define CHARACTERMANAGER_HPP_

#include "character_data.hpp"
#include "singleton.hpp"

#if defined(__MINGW32__)
 #include "sqlite3/sqlite3.h"
#else
 #include "sqlite3.h"
#endif

#include <functional>
#include <map>

class CharacterManager: public Singleton<CharacterManager> {
public:
	//common public methods
	int Create(int owner, std::string handle, std::string avatar);
	int Load(int owner, std::string handle, std::string avatar);
	int Save(int uid);
	void Unload(int uid);
	void Delete(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<const int, CharacterData>)> fn);

	//accessors and mutators
	CharacterData* Get(int uid);
	int GetLoadedCount();
	int GetTotalCount();
	std::map<int, CharacterData>* GetContainer();

	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();

private:
	friend Singleton<CharacterManager>;

	CharacterManager() = default;
	~CharacterManager() = default;

	//members
	std::map<int, CharacterData> elementMap;
	sqlite3* database = nullptr;
};

#endif
