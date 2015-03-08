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
#include "account_manager.hpp"

#include <stdexcept>

//-------------------------
//Define the queries
//-------------------------

static const char* CREATE_USER_ACCOUNT = "INSERT INTO Accounts (username) VALUES (?);";

static const char* LOAD_USER_ACCOUNT = "SELECT "
	"uid, "
	"blacklisted, "
	"whitelisted, "
	"mod, "
	"admin "
	" FROM Accounts WHERE username = ?;";

static const char* SAVE_USER_ACCOUNT = "UPDATE OR FAIL Accounts SET "
	"blacklisted = ?2, "
	"whitelisted = ?3, "
	"mod = ?4, "
	"admin = ?5 "
	"WHERE uid = ?1;";

static const char* DELETE_USER_ACCOUNT = "DELETE FROM Accounts WHERE uid = ?;";

static const char* COUNT_USER_ACCOUNT_RECORDS = "SELECT COUNT(*) FROM Accounts;";

//-------------------------
//Define the public methods
//-------------------------

//TODO: (1) block blacklisted accounts
int AccountManager::Create(std::string username, int clientIndex) {
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
	return Load(username, clientIndex);
}

int AccountManager::Load(std::string username, int clientIndex) {
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
		if (elementMap.find(uid) != elementMap.end()) {
			sqlite3_finalize(statement);
			return -1;
		}

		//extract the data into memory
		AccountData& newAccount = elementMap[uid];
		newAccount.username = username;
		newAccount.blackListed = sqlite3_column_int(statement, 1);
		newAccount.whiteListed = sqlite3_column_int(statement, 2);
		newAccount.mod = sqlite3_column_int(statement, 3);
		newAccount.admin = sqlite3_column_int(statement, 4);
		newAccount.clientIndex = clientIndex;

		//finish the routine
		sqlite3_finalize(statement);
		return uid;
	}

	sqlite3_finalize(statement);

	if (ret == SQLITE_DONE) {
		//create the non-existant account instead
		return Create(username, clientIndex);
	}

	throw(std::runtime_error(std::string() + "Unknown SQL error in LoadAccount: " + sqlite3_errmsg(database) ));
}

int AccountManager::Save(int uid) {
	//save this user account from memory, replacing it if it exists in the database
	//DOCS: To use this method, change the in-memory copy, and then call this function using that object's UID.

	//this method fails if this account is not loaded
	if (elementMap.find(uid) == elementMap.end()) {
		return -1;
	}

	AccountData& account = elementMap[uid];
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, SAVE_USER_ACCOUNT, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//parameters
	bool ret = false;
	ret |= sqlite3_bind_int(statement, 1, uid) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 2, account.blackListed) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 3, account.whiteListed) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 4, account.mod) != SQLITE_OK;
	ret |= sqlite3_bind_int(statement, 5, account.admin) != SQLITE_OK;

	//check for binding errors
	if (ret) {
		throw( std::runtime_error(std::string() + "Failed to replace a prepared statement's parameter: " + sqlite3_errmsg(database)) );
	}

	//execute
	if (sqlite3_step(statement) != SQLITE_DONE) {
		//if this fails, than something went horribly wrong
		sqlite3_finalize(statement);
		throw( std::runtime_error(std::string() + "Unknown SQL error when saving an account: " + sqlite3_errmsg(database)) );
	}

	sqlite3_finalize(statement);

	//successful execution
	return 0;
}

void AccountManager::Unload(int uid) {
	//save this user account, and then unload it
	//NOTE: the associated characters are unloaded externally
	Save(uid);
	elementMap.erase(uid);
}

void AccountManager::Delete(int uid) {
	//delete a user account from the database, and remove it from memory
	//NOTE: the associated characters should be deleted externally
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, DELETE_USER_ACCOUNT, -1, &statement, nullptr) != SQLITE_OK) {
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
		throw( std::runtime_error(std::string() + "Unknown SQL error when deleting an account: " + sqlite3_errmsg(database)) );
	}

	//finish the routine
	sqlite3_finalize(statement);
	elementMap.erase(uid);
}

void AccountManager::UnloadAll() {
	for (auto& it : elementMap) {
		Save(it.first);
	}
	elementMap.clear();
}

void AccountManager::UnloadIf(std::function<bool(std::pair<const int, AccountData const&>)> fn) {
	//replicate std::remove_if, using custom code
	std::map<int, AccountData>::iterator it = elementMap.begin();
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

AccountData* AccountManager::Get(int uid) {
	std::map<int, AccountData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int AccountManager::GetLoadedCount() {
	return elementMap.size();
}

int AccountManager::GetTotalCount() {
	//a lot just to count something.
	sqlite3_stmt* statement = nullptr;

	//prep
	if (sqlite3_prepare_v2(database, COUNT_USER_ACCOUNT_RECORDS, -1, &statement, nullptr) != SQLITE_OK) {
		throw( std::runtime_error(std::string() + "Failed to prepare an SQL statement: " + sqlite3_errmsg(database)) );
	}

	//execute & retrieve the result
	sqlite3_step(statement);
	int ret = sqlite3_column_int(statement, 0);

	//finish the routine
	sqlite3_finalize(statement);
	return ret;
}

std::map<int, AccountData>* AccountManager::GetContainer() {
	return &elementMap;
}

sqlite3* AccountManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* AccountManager::GetDatabase() {
	return database;
}
