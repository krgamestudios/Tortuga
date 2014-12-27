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
#include "tile_sheet_api.hpp"

#include "tile_sheet.hpp"

static int load(lua_State* L) {
	TileSheet* sheet = reinterpret_cast<TileSheet*>(lua_touserdata(L, 1));
	sheet->Load(lua_tostring(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4));
	return 0;
}

static int unload(lua_State* L) {
	TileSheet* sheet = reinterpret_cast<TileSheet*>(lua_touserdata(L, 1));
	sheet->Unload();
	return 0;
}

static int getXCount(lua_State* L) {
	TileSheet* sheet = reinterpret_cast<TileSheet*>(lua_touserdata(L, 1));
	lua_pushinteger(L, sheet->GetXCount());
	return 1;
}

static int getYCount(lua_State* L) {
	TileSheet* sheet = reinterpret_cast<TileSheet*>(lua_touserdata(L, 1));
	lua_pushinteger(L, sheet->GetYCount());
	return 1;
}

static int getTileW(lua_State* L) {
	TileSheet* sheet = reinterpret_cast<TileSheet*>(lua_touserdata(L, 1));
	lua_pushinteger(L, sheet->GetTileW());
	return 1;
}

static int getTileH(lua_State* L) {
	TileSheet* sheet = reinterpret_cast<TileSheet*>(lua_touserdata(L, 1));
	lua_pushinteger(L, sheet->GetTileH());
	return 1;
}

static const luaL_Reg tileSheetLib[] = {
	{"Load",load},
	{"Unload",unload},
	{"GetXCount",getXCount},
	{"GetYCount",getYCount},
	{"GetTileW",getTileW},
	{"GetTileH",getTileH},
	{nullptr, nullptr}
};

LUAMOD_API int openTileSheetAPI(lua_State* L) {
	luaL_newlib(L, tileSheetLib);
	return 1;
}