/* Copyright: (c) Kayne Ruse 2014
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

#include "sqlite3/sqlite3.h"

#include <map>
#include <functional>

class CharacterManager : public Singleton<CharacterManager> {
public:
	//public access methods
	int CreateCharacter(int owner, std::string handle, std::string avatar);
	int LoadCharacter(int owner, std::string handle, std::string avatar);
	int SaveCharacter(int uid);
	void UnloadCharacter(int uid);
	void DeleteCharacter(int uid);

	void UnloadCharacterIf(std::function<bool(std::map<int, CharacterData>::iterator)> f);

	void UnloadAll();

	//accessors and mutators
	CharacterData* GetCharacter(int uid);
	std::map<int, CharacterData>* GetContainer();

	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();

private:
	friend Singleton<CharacterManager>;

	CharacterManager() = default;
	~CharacterManager() = default;

	std::map<int, CharacterData> characterMap;
	sqlite3* database = nullptr;
};

#endif
