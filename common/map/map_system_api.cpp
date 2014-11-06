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
#include "map_system_api.hpp"

//all map API headers
#include "region_api.hpp"
#include "region_pager_api.hpp"
#include "tile_sheet_api.hpp"

//macros
#include "region.hpp"

//useful "globals"
static int getRegionWidth(lua_State* L) {
	lua_pushinteger(L, REGION_WIDTH);
	return 1;
}

static int getRegionHeight(lua_State* L) {
	lua_pushinteger(L, REGION_HEIGHT);
	return 1;
}

static int getRegionDepth(lua_State* L) {
	lua_pushinteger(L, REGION_DEPTH);
	return 1;
}

//This mimics linit.c to create a nested collection of all map modules.
static const luaL_Reg funcs[] = {
	//synonyms
	{"GetRegionWidth", getRegionWidth},
	{"GetRegionHeight", getRegionHeight},
	{"GetRegionDepth", getRegionDepth},
	{nullptr, nullptr}
};

static const luaL_Reg libs[] = {
	{"Region", openRegionAPI},
	{"RegionPager", openRegionPagerAPI},
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