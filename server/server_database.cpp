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

static const char* CREATE_USER_ACCOUNT = "INSERT INTO UserAccounts (username) VALUES (?);";
static const char* LOAD_USER_ACCOUNT = "SELECT * FROM UserAccounts WHERE username = ?;";

//-------------------------
//Define the methods
//-------------------------

int ServerApplication::CreateUserAccount(std::string username, int clientIndex) {
	//create this user account, failing if it exists, leave this account in memory
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, CREATE_USER_ACCOUNT, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameter
	if (sqlite3_bind_text(statement, 1, username.c_str(), username.size() + 1, SQLITE_STATIC) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	if (sqlite3_step(statement) != SQLITE_DONE) {
		//if this fails, than this account exists
		sqlite3_finalize(statement);
		return -1;
	}

	sqlite3_finalize(statement);

	//load this account into memory
	return LoadUserAccount(username, clientIndex);
}

int ServerApplication::LoadUserAccount(std::string username, int clientIndex) {
	//load this user account, failing if it is in memory, creating it if it doesn't exist
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, LOAD_USER_ACCOUNT, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameter
	if (sqlite3_bind_text(statement, 1, username.c_str(), username.size() + 1, SQLITE_STATIC) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	int ret = sqlite3_step(statement);

	//process the result
	if (ret == SQLITE_ROW) {
		//get the index
		int uid = sqlite3_column_int(statement, 0);

		//check to see if this account is already loaded
		if (accountMap.find(uid) != accountMap.end()) {
			sqlite3_finalize(statement);
			return -1;
		}

		//extract the data into memory
		AccountData& newAccount = accountMap[uid];
		newAccount.username = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
		newAccount.blackListed = sqlite3_column_int(statement, 2);
		newAccount.whiteListed = sqlite3_column_int(statement, 3);
		newAccount.clientIndex = clientIndex;

		//finish the routine
		sqlite3_finalize(statement);
		return uid;
	}

	sqlite3_finalize(statement);

	if (ret == SQLITE_DONE) {
		//create the non-existant account instead
		return CreateUserAccount(username, clientIndex);
	}

	throw(std::runtime_error(std::string() + "Unknown SQL error in LoadUserAccount: " + sqlite3_errmsg(database) ));
}

void ServerApplication::SaveUserAccount(std::string username) {
	//save this user account, replacing it if it exists
	//TODO
}

void ServerApplication::UnloadUserAccount(std::string username) {
	//save this user account, and then unload it
	//TODO
}

void ServerApplication::DeleteUserAccount(std::string username) {
	//delete a user account from the database, and remove it from memory
	//TODO
}
