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

static int index(lua_State* L) {
	std::cout << "WARNING: userdataIndex called" << std::endl;
	return 0;
}

static luaL_Reg metatable[] = {
	{"__index", index},
	{nullptr, nullptr}
};

int createUserdataMetatable(lua_State* L) {
	//create the userdata metatable
	lua_pushlightuserdata(L, nullptr); //anon userdata
	lua_createtable(L, 0, 0); //table

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