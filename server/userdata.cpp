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
#include "userdata.hpp"

#include <iostream>

#define METAMETA "__metameta"

static int index(lua_State* L) {
	std::cout << "debug: index" << std::endl;

	//get __metameta
	lua_getmetatable(L, 1); //get the userdata metatable
	lua_pushstring(L, METAMETA); //push the __metameta name
	lua_gettable(L, -2); //get the __metameta field from the metatable

	//get __metameta[userdata]
	lua_pushvalue(L, 1); //copy of the userdata
	lua_gettable(L, -2); //get the table at __metameta[userdata]

	//table doesn't exist yet
	if (lua_isnil(L, -1)) {
		std::cout << "debug: index (no table)" << std::endl;
		return 1;
	}

	//get the value at __metameta[userdata][key]
	lua_pushvalue(L, 2); //copy the key
	lua_gettable(L, -2); //get value from the deepest table

	//return
	return 1;
}

static int newindex(lua_State* L) {
	std::cout << "debug: newindex" << std::endl;

	//get __metameta
	lua_getmetatable(L, 1); //get the userdata metatable
	lua_pushstring(L, METAMETA); //push the __metameta name
	lua_gettable(L, -2); //get the __metameta field from the metatable

	//get __metameta[userdata]
	lua_pushvalue(L, 1); //copy of the userdata
	lua_gettable(L, -2); //get the table at __metameta[userdata]

	//if this table doesn't exist yet
	if (lua_isnil(L, -1)) {
		std::cout << "debug: newindex (new table)" << std::endl;
		lua_pop(L, 1); //pop nil

		lua_pushvalue(L, 1); //copy userdata (key)
		lua_createtable(L, 0, 0); //new table (value)
		lua_settable(L, -3); //create the new table in __metameta

		lua_pushvalue(L, 1); //copy userdata (key)
		lua_gettable(L, -2); //get the newly created table
	}

	//set the value at __metameta[userdata][key] = value
	lua_pushvalue(L, 2); //copy the key
	lua_pushvalue(L, 3); //copy the value
	lua_settable(L, -3); //set value in the deepest table

	//return
	return 0;
}

static luaL_Reg metatable[] = {
	{"__index", index},
	{"__newindex", newindex},
	{nullptr, nullptr}
};

int createUserdataMetatable(lua_State* L) {
	//create the userdata metatable
	lua_pushlightuserdata(L, nullptr); //anon userdata
	lua_createtable(L, 0, 0); //table

	//insert meta-metatable
	lua_pushstring(L, METAMETA);
	lua_createtable(L, 0, 0);
	lua_settable(L, -3);

	//insert metatable blocker
	lua_pushstring(L, "__metatable");
	lua_createtable(L, 0, 0);
	lua_settable(L, -3);

	//insert metamethods
	for (luaL_Reg* it = metatable; it->name; it++) {
		lua_pushstring(L, it->name);
		lua_pushcfunction(L, it->func);
		lua_settable(L, -3);
	}

	//set in the object & pop the anon userdata
	lua_setmetatable(L, -2); //set the metatable for userdata
	lua_pop(L, 1); //pop the anon userdata

	return 0;
}