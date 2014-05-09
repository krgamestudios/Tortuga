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
#include "server_application.hpp"

#include "sqlite3/sqlite3.h"

#include <stdexcept>

//-------------------------
//Define the queries
//-------------------------

static const char* CREATE_CHARACTER = "INSERT INTO PlayerCharacters (owner, handle, avatar) VALUES (?, ?, ?);";
static const char* LOAD_CHARACTER = "SELECT * FROM PlayerCharacters WHERE handle = ?;";
//static const char* SAVE_CHARACTER = ";";
//static const char* DELETE_CHARACTER = ";";

//-------------------------
//Define the methods
//-------------------------

int ServerApplication::CreateCharacter(int owner, std::string handle, std::string avatar) {
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

int ServerApplication::LoadCharacter(int owner, std::string handle, std::string avatar) {
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
		newChar.handle = reinterpret_cast<const char*>(sqlite3_column_text(statement, 2));
		newChar.avatar = reinterpret_cast<const char*>(sqlite3_column_text(statement, 3));
		//Don't cache the birth

		//world position
		newChar.mapIndex = sqlite3_column_int(statement, 5);
		newChar.position.x = (double)sqlite3_column_int(statement, 6);
		newChar.position.y = (double)sqlite3_column_int(statement, 7);

		//statistics
		newChar.level = sqlite3_column_int(statement, 8);
		newChar.exp = sqlite3_column_int(statement, 9);
		newChar.maxHP = sqlite3_column_int(statement, 10);
		newChar.health = sqlite3_column_int(statement, 11);
		newChar.maxMP = sqlite3_column_int(statement, 12);
		newChar.mana = sqlite3_column_int(statement, 13);
		newChar.attack = sqlite3_column_int(statement, 14);
		newChar.defence = sqlite3_column_int(statement, 15);
		newChar.intelligence = sqlite3_column_int(statement, 16);
		newChar.resistance = sqlite3_column_int(statement, 17);
		newChar.accuracy = sqlite3_column_double(statement, 18);
		newChar.evasion = sqlite3_column_double(statement, 19);
		newChar.luck = sqlite3_column_double(statement, 20);

		//TODO: equipment

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

void ServerApplication::SaveCharacter() {
	//TODO: save this character
}

void ServerApplication::UnloadCharacter() {
	//TODO: save this character, then unload it
}

void ServerApplication::DeleteCharacter() {
	//TODO:  delete this character, then remove it from memory
}
