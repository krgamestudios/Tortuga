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
#include "room_system_api.hpp"

//all map API headers
#include "room_api.hpp"
#include "room_manager_api.hpp"

//useful "globals"
//...

//This mimics linit.c to create a nested collection of all map modules.
static const luaL_Reg funcs[] = {
	{nullptr, nullptr}
};

static const luaL_Reg libs[] = {
	{"Room", openRoomAPI},
	{"RoomManager", openRoomManagerAPI},
//	{"TileSheet", openTileSheetAPI},
	{nullptr, nullptr}
};

int openMapSystemAPI(lua_State* L) {
	//create the table
	luaL_newlibtable(L, libs);

	//push the "global" functions
	luaL_setfuncs(L, funcs, 0);

	//push the substable
	for (const luaL_Reg* lib = libs; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	return 1;
}