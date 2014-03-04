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
#ifndef PLAYERMANAGER_HPP_
#define PLAYERMANAGER_HPP_

#include "vector2.hpp"

#include "sqlite3/sqlite3.h"

#include <functional>
#include <map>
#include <string>

struct PlayerEntry {
	int clientIndex;
	int mapIndex;
	Vector2 position;
	Vector2 motion;
};

class PlayerManager {
public:
	//clarity typedefs
	typedef std::map<int, PlayerEntry>		Container;
	typedef Container::iterator				Iterator;
	typedef std::function<void(Iterator)>	Lambda;

	//These functions interact with the database
	//*Deletion, *Load and *Unload returns: 0 success, -1 failure
	//*Creation returns the uniqueID, but doesn't load
	//  that object; call *Load directly afterward
	int HandlePlayerCreation	(std::string name, std::string avatar);
	int HandlePlayerDeletion	(int uniqueID);
	int HandlePlayerLoad		(int uniqueID, int clientIndex);
	int HandlePlayerUnload		(int uniqueID);

	//lambdas
	void ForEach(Lambda);

	//accessors
	PlayerEntry* GetPlayer(int uniqueID);
	sqlite3* SetDatabase(sqlite3* db) { return database = db; }
	sqlite3* GetDatabase() { return database; }

	//update each player's position
	void Update(double delta);
private:
	Container playerMap;
	sqlite3* database = nullptr;
};

#endif
