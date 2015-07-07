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
#include "character_manager.hpp"

#include "sqlite3.h"

#include "character_defines.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>

//-------------------------
//Define the queries
//-------------------------

//NOTE: Programmer set variables are NOT zero-indexed
//NOTE: SQLite3 returned variables (i.e. loading) ARE zero-indexed

static const char* CREATE_CHARACTER = "INSERT INTO LiveCharacters ("
	"owner, "
	"handle, "
	"avatar, "
	"boundsX, "
	"boundsY, "
	"boundsW, "
	"boundsH"
	") VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";

static const char* LOAD_CHARACTER = "SELECT "
	"uid, "
	"owner, "
	"handle, "
	"avatar, "
	"roomIndex, "
	"originX, "
	"originY, "
	"boundsX, "
	"boundsY, "
	"boundsW, "
	"boundsH "
	"FROM LiveCharacters WHERE handle = ?;";

static const char* SAVE_CHARACTER = "UPDATE OR FAIL LiveCharacters SET "
	"roomIndex = ?2, "
	"originX = ?3, "
	"originY = ?4, "
	"boundsX = ?5, "
	"boundsY = ?6, "
	"boundsW = ?7, "
	"boundsH = ?8 "
	"WHERE uid = ?1;";

static const char* DELETE_CHARACTER = "DELETE FROM LiveCharacters WHERE uid = ?;";

static const char* COUNT_CHARACTER_RECORDS = "SELECT COUNT(*) FROM LiveCharacters;";

//-------------------------
//Define the methods
//-------------------------

//NOTE: default baseStats as a parameter would be good for different beggining states or multiple classes
int CharacterManager::Create(int owner, std::string handle, std::string avatar) {
	//Create the character, failing if it exists
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, CREATE_CHARACTER, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameters
	bool ret = false;
	ret |= sqlite3_bind_int(statement, 1, owner);
	ret |= sqlite3_bind_text(statement, 2, handle.c_str(), handle.size() + 1, SQLITE_STATIC);
	ret |= sqlite3_bind_text(statement, 3, avatar.c_str(), avatar.size() + 1, SQLITE_STATIC);
	ret |= sqlite3_bind_int(statement, 4, CHARACTER_BOUNDS_X);
	ret |= sqlite3_bind_int(statement, 5, CHARACTER_BOUNDS_Y);
	ret |= sqlite3_bind_int(statement, 6, CHARACTER_BOUNDS_WIDTH);
	ret |= sqlite3_bind_int(statement, 7, CHARACTER_BOUNDS_HEIGHT);

	//check for binding errors
	if (ret) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	if (sqlite3_step(statement) != SQLITE_DONE) {
		//if this fails, than this character exists
		sqlite3_finalize(statement);
		return -1;
	}

	sqlite3_finalize(statement);

	//load this character into memory
	return Load(owner, handle, avatar);
}

int CharacterManager::Load(int owner, std::string handle, std::string avatar) {
	//load the specified character, creating it if it doesn't exist
	//fail if it is already loaded, or does not belong to this account
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, LOAD_CHARACTER, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameter
	if (sqlite3_bind_text(statement, 1, handle.c_str(), handle.size() + 1, SQLITE_STATIC) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	int ret = sqlite3_step(statement);

	//process the result
	if (ret == SQLITE_ROW) {
		//get the index
		int uid = sqlite3_column_int(statement, 0);

		//check to see if this character is already loaded
		if (elementMap.find(uid) != elementMap.end()) {
			sqlite3_finalize(statement);
			return -1;
		}

		//check the owner
		if (owner != sqlite3_column_int(statement, 1)) {
			sqlite3_finalize(statement);
			//unload the already loaded character
			Unload(uid);
			return -2;
		}

		//extract the data into memory
		CharacterData& newChar = elementMap[uid];

		//metadata
		newChar.owner = owner;
		newChar.handle = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
		newChar.avatar = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
		//Don't cache the birth

		//world origin
		newChar.roomIndex = sqlite3_column_int(statement, 4);
		newChar.origin.x = (double)sqlite3_column_int(statement, 5);
		newChar.origin.y = (double)sqlite3_column_int(statement, 6);
		//bounds
		newChar.bounds.x = (int)sqlite3_column_int(statement, 7);
		newChar.bounds.y = (int)sqlite3_column_int(statement, 8);
		newChar.bounds.w = (int)sqlite3_column_int(statement, 9);
		newChar.bounds.h = (int)sqlite3_column_int(statement, 10);

		//gameplay components: equipment, items, buffs, debuffs...

		//finish the routine
		sqlite3_finalize(statement);
		return uid;
	}

	sqlite3_finalize(statement);

	if (ret == SQLITE_DONE) {
		//create the non-existant character instead
		return Create(owner, handle, avatar);
	}

	throw(std::runtime_error(std::string() + "Unknown SQL error in LoadCharacter: " + sqlite3_errmsg(database) ));
}

int CharacterManager::Save(int uid) {
	//save this character from memory, replacing it if it exists in the database
	//DOCS: To use this method, change the in-memory copy, and then call this function using that object's UID.

	//this method fails if this character is not loaded
	if (elementMap.find(uid) == elementMap.end()) {
		return -1;
	}

	CharacterData& character = elementMap[uid];
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, SAVE_CHARACTER, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameters
	bool ret = false;
	ret |= sqlite3_bind_int(statement, 1, uid) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 2, character.roomIndex) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 3, (int)character.origin.x) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 4, (int)character.origin.y) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 5, character.bounds.x) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 6, character.bounds.y) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 7, character.bounds.w) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 8, character.bounds.h) != SQLITE_OK;

	//gameplay components: equipment, items, buffs, debuffs...

	//check for binding errors
	if (ret) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	if (sqlite3_step(statement) != SQLITE_DONE) {
		//if this fails, than something went horribly wrong
		sqlite3_finalize(statement);
		throw( std::runtime_error(std::string() + "Unknown SQL error when saving a character: " + sqlite3_errmsg(database)) );
	}

	sqlite3_finalize(statement);

	//successful execution
	return 0;
}

void CharacterManager::Unload(int uid) {
	//save this character, then unload it
	Save(uid);
	elementMap.erase(uid);
}

void CharacterManager::Delete(int uid) {
	//TODO: when deleting a character, move it to an archive table
	//delete this character from the database, then remove it from memory
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, DELETE_CHARACTER, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameter
	if (sqlite3_bind_int(statement, 1, uid) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	if (sqlite3_step(statement) != SQLITE_DONE) {
		//if this fails, than something went horribly wrong
		sqlite3_finalize(statement);
		throw( std::runtime_error(std::string() + "Unknown SQL error when deleting a character: " + sqlite3_errmsg(database)) );
	}

	//finish the routine
	sqlite3_finalize(statement);
	elementMap.erase(uid);
}

void CharacterManager::UnloadAll() {
	for (auto& it : elementMap) {
		Save(it.first);
	}
	elementMap.clear();
}

void CharacterManager::UnloadIf(std::function<bool(std::pair<const int, CharacterData const&>)> fn) {
	std::map<int, CharacterData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			Save(it->first);
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

//-------------------------
//Define the accessors and mutators
//-------------------------

CharacterData* CharacterManager::Get(int uid) {
	std::map<int, CharacterData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

CharacterData* CharacterManager::Get(std::string handle) {
	for (std::map<int, CharacterData>::iterator it = elementMap.begin(); it != elementMap.end(); ++it) {
		if (it->second.GetHandle() == handle) {
			return &it->second;
		}
	}
	return nullptr;
}

int CharacterManager::GetLoadedCount() {
	return elementMap.size();
}

int CharacterManager::GetTotalCount() {
	//a lot just to count something.
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, COUNT_CHARACTER_RECORDS, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//execute & retrieve the result
	sqlite3_step(statement);
	int ret = sqlite3_column_int(statement, 0);

	//finish the routine
	sqlite3_finalize(statement);
	return ret;
}

std::map<int, CharacterData>* CharacterManager::GetContainer() {
	return &elementMap;
}

sqlite3* CharacterManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* CharacterManager::GetDatabase() {
	return database;
}
