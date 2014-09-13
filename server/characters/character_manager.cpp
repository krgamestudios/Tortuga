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
#include "character_manager.hpp"

#include "sqlite3/sqlite3.h"

#include <stdexcept>

//-------------------------
//Define the queries
//-------------------------

static const char* CREATE_CHARACTER = "INSERT INTO Characters (owner, handle, avatar) VALUES (?, ?, ?);";
static const char* LOAD_CHARACTER = "SELECT * FROM Characters WHERE handle = ?;";

static const char* SAVE_CHARACTER = "UPDATE OR FAIL Characters SET "
	"roomIndex = ?2,"
	"originX = ?3,"
	"originY = ?4,"
	"level = ?5,"
	"exp = ?6,"
	"maxHP = ?7,"
	"health = ?8,"
	"maxMP = ?9,"
	"mana = ?10,"
	"attack = ?11,"
	"defence = ?12,"
	"intelligence = ?13,"
	"resistance = ?14,"
	"speed = ?15,"
	"accuracy = ?16,"
	"evasion = ?17,"
	"luck = ?18"
" WHERE uid = ?1;";

static const char* DELETE_CHARACTER = "DELETE FROM Characters WHERE uid = ?;";

//-------------------------
//Define the methods
//-------------------------

//NOTE: default baseStats as a parameter would be good for different beggining states or multiple classes
int CharacterManager::CreateCharacter(int owner, std::string handle, std::string avatar) {
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
	return LoadCharacter(owner, handle, avatar);
}

int CharacterManager::LoadCharacter(int owner, std::string handle, std::string avatar) {
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
		if (characterMap.find(uid) != characterMap.end()) {
			sqlite3_finalize(statement);
			return -1;
		}

		//check the owner
		if (owner != sqlite3_column_int(statement, 1)) {
			sqlite3_finalize(statement);
			return -2;
		}

		//extract the data into memory
		CharacterData& newChar = characterMap[uid];

		//metadata
		newChar.owner = owner;
		newChar.handle = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
		newChar.avatar = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
		//Don't cache the birth

		//world origin
		newChar.roomIndex = sqlite3_column_int(statement, 5);
		newChar.origin.x = (double)sqlite3_column_int(statement, 6);
		newChar.origin.y = (double)sqlite3_column_int(statement, 7);

		//statistics
		newChar.baseStats.level = sqlite3_column_int(statement, 8);
		newChar.baseStats.exp = sqlite3_column_int(statement, 9);
		newChar.baseStats.maxHP = sqlite3_column_int(statement, 10);
		newChar.baseStats.health = sqlite3_column_int(statement, 11);
		newChar.baseStats.maxMP = sqlite3_column_int(statement, 12);
		newChar.baseStats.mana = sqlite3_column_int(statement, 13);
		newChar.baseStats.attack = sqlite3_column_int(statement, 14);
		newChar.baseStats.defence = sqlite3_column_int(statement, 15);
		newChar.baseStats.intelligence = sqlite3_column_int(statement, 16);
		newChar.baseStats.resistance = sqlite3_column_int(statement, 17);
		newChar.baseStats.speed = sqlite3_column_int(statement, 18);
		newChar.baseStats.accuracy = sqlite3_column_double(statement, 19);
		newChar.baseStats.evasion = sqlite3_column_double(statement, 20);
		newChar.baseStats.luck = sqlite3_column_double(statement, 21);

		//gameplay components: equipment, items, buffs, debuffs...

		//finish the routine
		sqlite3_finalize(statement);
		return uid;
	}

	sqlite3_finalize(statement);

	if (ret == SQLITE_DONE) {
		//create the non-existant character instead
		return CreateCharacter(owner, handle, avatar);
	}

	throw(std::runtime_error(std::string() + "Unknown SQL error in LoadCharacter: " + sqlite3_errmsg(database) ));
}

int CharacterManager::SaveCharacter(int uid) {
	//save this character from memory, replacing it if it exists in the database
	//DOCS: To use this method, change the in-memory copy, and then call this function using that object's UID.

	//this method fails if this character is not loaded
	if (characterMap.find(uid) == characterMap.end()) {
		return -1;
	}

	CharacterData& character = characterMap[uid];
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

	//statistics
	ret |= sqlite3_bind_int(statement, 5, character.baseStats.level) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 6, character.baseStats.exp) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 7, character.baseStats.maxHP) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 8, character.baseStats.health) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 9, character.baseStats.maxMP) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 10, character.baseStats.mana) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 11, character.baseStats.attack) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 12, character.baseStats.defence) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 13, character.baseStats.intelligence) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 14, character.baseStats.resistance) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 15, character.baseStats.speed) != SQLITE_OK;
	ret |= sqlite3_bind_double(statement, 16, character.baseStats.accuracy) != SQLITE_OK;
	ret |= sqlite3_bind_double(statement, 17, character.baseStats.evasion) != SQLITE_OK;
	ret |= sqlite3_bind_double(statement, 18, character.baseStats.luck) != SQLITE_OK;

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

void CharacterManager::UnloadCharacter(int uid) {
	//save this character, then unload it
	SaveCharacter(uid);
	characterMap.erase(uid);
}

void CharacterManager::DeleteCharacter(int uid) {
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
	characterMap.erase(uid);
}

void CharacterManager::UnloadCharacterIf(std::function<bool(std::map<int, CharacterData>::iterator)> f) {
	//save this character, then unload it if the parameter returns true
	for (std::map<int, CharacterData>::iterator it = characterMap.begin(); it != characterMap.end(); /* EMPTY */ ) {
		if (f(it)) {
			SaveCharacter(it->first);
			it = characterMap.erase(it);
			continue;
		}
		it++;
	}
}

void CharacterManager::UnloadAll() {
	for (auto& it : characterMap) {
		SaveCharacter(it.first);
	}
	characterMap.clear();
}

//-------------------------
//Define the accessors and mutators
//-------------------------

CharacterData* CharacterManager::GetCharacter(int uid) {
	std::map<int, CharacterData>::iterator it = characterMap.find(uid);

	if (it == characterMap.end()) {
		return nullptr;
	}

	return &it->second;
}

std::map<int, CharacterData>* CharacterManager::GetContainer() {
	return &characterMap;
}

sqlite3* CharacterManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* CharacterManager::GetDatabase() {
	return database;
}