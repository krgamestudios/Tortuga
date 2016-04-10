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
#include "barrier_manager_api.hpp"

#include "barrier_manager.hpp"

static int setOnCreate(lua_State* L) {
	BarrierManager* barrierMgr = static_cast<BarrierManager*>(lua_touserdata(L, 1));
	barrierMgr->SetCreateReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static int setOnUnload(lua_State* L) {
	BarrierManager* barrierMgr = static_cast<BarrierManager*>(lua_touserdata(L, 1));
	barrierMgr->SetUnloadReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static const luaL_Reg barrierManagerLib[] = {
	{"SetOnCreate", setOnCreate},
	{"SetOnUnload", setOnUnload},
	{nullptr, nullptr}
};

LUAMOD_API int openBarrierManagerAPI(lua_State* L) {
	luaL_newlib(L, barrierManagerLib);
	return 1;
}