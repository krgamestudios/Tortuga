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
#include "waypoint_api.hpp"

#include "waypoint_data.hpp"

//origin
static int setOriginX(lua_State* L) {
	//TODO
	return 0;
}

static int setOriginY(lua_State* L) {
	//TODO
	return 0;
}

static int getOriginX(lua_State* L) {
	//TODO
	return 0;
}

static int getOriginY(lua_State* L) {
	//TODO
	return 0;
}

//bounds
static int setBoundingBoxX(lua_State* L) {
	//TODO
	return 0;
}

static int setBoundingBoxY(lua_State* L) {
	//TODO
	return 0;
}

static int setBoundingBoxW(lua_State* L) {
	//TODO
	return 0;
}

static int setBoundingBoxH(lua_State* L) {
	//TODO
	return 0;
}

static int getBoundingBoxX(lua_State* L) {
	//TODO
	return 0;
}

static int getBoundingBoxY(lua_State* L) {
	//TODO
	return 0;
}

static int getBoundingBoxW(lua_State* L) {
	//TODO
	return 0;
}

static int getBoundingBoxH(lua_State* L) {
	//TODO
	return 0;
}

//triggers
static int setTriggerReference(lua_State* L) {
	//TODO
	return 0;
}

static int getTriggerReference(lua_State* L) {
	//TODO
	return 0;
}

static const luaL_Reg waypointLib[] = {
	//origin
	{"SetOriginX",setOriginX},
	{"SetOriginY",setOriginY},
	{"GetOriginX",getOriginX},
	{"GetOriginY",getOriginY},

	//bounds
	{"SetBoundsX",setBoundingBoxX},
	{"SetBoundsY",setBoundingBoxY},
	{"SetBoundsW",setBoundingBoxW},
	{"SetBoundsH",setBoundingBoxH},

	{"GetBoundsX",getBoundingBoxX},
	{"GetBoundsY",getBoundingBoxY},
	{"GetBoundsW",getBoundingBoxW},
	{"GetBoundsH",getBoundingBoxH},

	//triggers
	{"SetTrigger",setTriggerReference},
	{"GetTrigger",getTriggerReference},
	{nullptr, nullptr}
};

LUAMOD_API int openWaypointAPI(lua_State* L) {
	luaL_newlib(L, waypointLib);
	return 1;
}