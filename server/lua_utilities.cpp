/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "lua_utilities.hpp"

#include <stdexcept>
#include <string>

//this is for functions passed to SetOnCreate & SetOnUnload
int runHook(lua_State* L, int scriptRef, void* userdata, int index) {
	//get the hook
	lua_rawgeti(L, LUA_REGISTRYINDEX, scriptRef);

	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return -1;
	}

	//push args
	lua_pushlightuserdata(L, userdata);
	lua_pushinteger(L, index);

	//call the function, 2 arg, 0 return
	if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
		throw(std::runtime_error(std::string() + "Lua error: " + lua_tostring(L, -1) ));
	}

	return 0;
}