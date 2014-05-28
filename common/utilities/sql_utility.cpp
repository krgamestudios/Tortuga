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
#include "sql_utility.hpp"

#include "utility.hpp"

#include <stdexcept>
#include <fstream>
#include <cstdlib>

int runSQLScript(sqlite3* db, std::string fname, int (*callback)(void*,int,char**,char**), void* argPtr) {
	//load the file into a string
	std::ifstream is(fname);
	if (!is.is_open()) {
		return -1;
	}
	std::string script;
	getline(is, script, '\0');
	is.close();

	//run the SQL loaded from the file
	char* errmsg = nullptr;
	int ret = sqlite3_exec(db, script.c_str(), callback, argPtr, &errmsg);
	if (ret != SQLITE_OK) {
		//handle any errors received from the SQL
		std::runtime_error e(std::string() + "SQL Script Error " + to_string_custom(ret) + ": " + errmsg);
		free(errmsg);
		throw(e);
	}
	return ret;
}