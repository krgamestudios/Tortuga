/* Copyright: (c) Kayne Ruse 2013
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

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>

ServerApplication ServerApplication::instance;

ServerApplication::ServerApplication() {
	//TODO
}

ServerApplication::~ServerApplication() {
	//TODO
}

void ServerApplication::Init(int argc, char** argv) {
	//Check thread safety
	if (!sqlite3_threadsafe()) {
		throw(std::runtime_error("Cannot run without thread safety"));
	}
	else {
		std::cout << "Thread safety confirmed" << std::endl;
	}

	//Init SDL
	if (SDL_Init(0)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	else {
		std::cout << "SDL initialized" << std::endl;
	}

	//Init lua
	if (!(luaState = luaL_newstate())) {
		throw(std::runtime_error("Failed to create the lua state"));
	}
	else {
		std::cout << "lua initialized" << std::endl;
	}
	luaL_openlibs(luaState);

	//Init SQL
	std::string dbname = (argc > 1) ? argv[1] : argv[0];
	int ret = sqlite3_open_v2((dbname + ".db").c_str(), &database, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, nullptr);
	if (ret != SQLITE_OK || !database) {
		throw(std::runtime_error("Failed to open the server database"));
	}
	else {
		std::cout << "Database filename: \"" << dbname << ".db\"" << std::endl;
	}

	//Run setup scripts
	luaL_dofile(luaState, "rsc/setup_server.lua");

	std::ifstream is("rsc/setup_server.sql");
	if (!is.is_open()) {
		throw(std::runtime_error("Failed to run database setup script"));
	}
	else {
		std::cout << "Running the database script" << std::endl;
	}
	std::string script;
	getline(is, script, '\0');
	is.close();

	sqlite3_exec(database, script.c_str(), nullptr, nullptr, nullptr);
}

void ServerApplication::Loop() {
	//TODO
}

void ServerApplication::Quit() {
	sqlite3_close_v2(database);
	lua_close(luaState);
	SDL_Quit();
}
