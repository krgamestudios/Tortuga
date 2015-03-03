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
#include "trigger_manager_api.hpp"

#include "trigger_manager.hpp"

//TODO: (1) figure out a way to iterate through elements of managers from lua

static int create(lua_State* L) {
	//DOCS: params: create(triggerMgr, name[, originX, originY[, boundsX, boundsY, boundsW, boundsH]][, script])

	//get the trigger manager
	TriggerManager* mgr = static_cast<TriggerManager*>(lua_touserdata(L, 1));

	//create the trigger
	int index = mgr->Create(lua_tostring(L, 2));
	TriggerData* triggerData = mgr->Get(index);

	//origin
	if (lua_gettop(L) >= 4) {
		triggerData->SetOrigin({lua_tonumber(L, 3), lua_tonumber(L, 4)}); //vectorX, vectorY
	}

	//bounds
	if (lua_gettop(L) >= 8) {
		triggerData->SetBoundingBox({
			lua_tointeger(L, 5), //boundsX
			lua_tointeger(L, 6), //boundsY
			lua_tointeger(L, 7), //boundsW
			lua_tointeger(L, 8)  //boundsH
		});
	}

	//if the parameter list isn't capped with a script, append a nil instead
	if (lua_type(L, -1) != LUA_TFUNCTION) {
		lua_pushnil(L);
	}

	//set the script reference (may be nil)
	triggerData->SetScriptReference(luaL_ref(L, LUA_REGISTRYINDEX));

	//push to the scipts
	lua_pushlightuserdata(L, static_cast<void*>(triggerData));
	lua_pushinteger(L, index);

	return 2;
}

static int unload(lua_State* L) {
	TriggerManager* mgr = static_cast<TriggerManager*>(lua_touserdata(L, 1));
	int count = 0; //the number removed

	//based on the type
	switch(lua_type(L, 2)) {
		//unload this index
		case LUA_TNUMBER:
			mgr->UnloadIf([L, &count](std::pair<int, TriggerData const&> it) -> bool {
				if (it.first == lua_tointeger(L, 2)) {
					count++;
					return true;
				}
				else {
					return false;
				}
			});
		break;

		//unload this name
		case LUA_TSTRING:
			mgr->UnloadIf([L, &count](std::pair<int, TriggerData const&> it) -> bool {
				if (it.second.GetHandle() == lua_tostring(L, 2)) {
					count++;
					return true;
				}
				else {
					return false;
				}
			});
		break;
	}

	//return the number removed
	lua_pushinteger(L, count);
	return 1;
}

static int getTrigger(lua_State* L) {
	TriggerManager* mgr = static_cast<TriggerManager*>(lua_touserdata(L, 1));
	TriggerData* triggerData = nullptr;

	switch(lua_type(L, 2)) {
		case LUA_TNUMBER:
			triggerData = mgr->Get(lua_tointeger(L, 2));
		break;
		case LUA_TSTRING:
			triggerData = mgr->Get(lua_tostring(L, 2));
		break;
	}

	if (triggerData) {
		lua_pushlightuserdata(L, static_cast<void*>(triggerData));
	}
	else {
		lua_pushnil(L);
	}

	return 1;
}

static int forEach(lua_State* L) {
	//TODO: (9) forEach()
}

static int getLoadedCount(lua_State* L) {
	TriggerManager* mgr = static_cast<TriggerManager*>(lua_touserdata(L, 1));
	lua_pushinteger(L, mgr->GetLoadedCount());
	return 1;
}

static const luaL_Reg triggerManagerLib[] = {
	{"Create",create},
	{"Unload",unload},
	{"GetTrigger",getTrigger},
	{"GetCount",getLoadedCount},
	{nullptr, nullptr}
};

LUAMOD_API int openTriggerManagerAPI(lua_State* L) {
	luaL_newlib(L, triggerManagerLib);
	return 1;
}