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
#ifndef ACCOUNTMANAGER_HPP_
#define ACCOUNTMANAGER_HPP_

#include "account_data.hpp"
#include "singleton.hpp"

#include "sqlite3/sqlite3.h"

#include <functional>
#include <map>

class AccountManager : public Singleton<AccountManager> {
public:
	//public access methods
	int Create(std::string username, int clientIndex);
	int Load(std::string username, int clientIndex);
	int Save(int uid);
	void Unload(int uid);
	void Delete(int uid);

	void UnloadAll();
	void UnloadIf(std::function<bool(std::pair<int, AccountData>)> fn);

	//accessors and mutators
	AccountData* Get(int uid);
	int GetLoadedCount();
	int GetTotalCount();
	std::map<int, AccountData>* GetContainer();

	sqlite3* SetDatabase(sqlite3* db);
	sqlite3* GetDatabase();

private:
	friend Singleton<AccountManager>;

	AccountManager() = default;
	~AccountManager() = default;

	std::map<int, AccountData> accountMap;
	sqlite3* database = nullptr;
};

#endif