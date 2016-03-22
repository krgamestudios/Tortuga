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
#pragma once

#include "region_pager_base.hpp"

#include "lua.hpp"

#include <functional>
#include <string>

//DOCS: set the lua hook before use

class RegionPagerLua : public RegionPagerBase {
public:
	RegionPagerLua() = default;
	~RegionPagerLua();

	//region manipulation
	Region* LoadRegion(int x, int y) override;
	Region* SaveRegion(int x, int y) override;
	Region* CreateRegion(int x, int y) override;

	void UnloadIf(std::function<bool(Region const&)> fn) override;
	void UnloadAll() override;

	//accessors & mutators
	lua_State* SetLuaState(lua_State* L) { return lua = L; }
	lua_State* GetLuaState() { return lua; }

	//utilities for the API
	int SetLoadReference(int i) { return loadRef = i; }
	int SetSaveReference(int i) { return saveRef = i; }
	int SetCreateReference(int i) { return createRef = i; }
	int SetUnloadReference(int i) { return unloadRef = i; }

	int GetLoadReference() { return loadRef; }
	int GetSaveReference() { return saveRef; }
	int GetCreateReference() { return createRef; }
	int GetUnloadReference() { return unloadRef; }

protected:
	lua_State* lua = nullptr;

	int loadRef = LUA_NOREF;
	int saveRef = LUA_NOREF;
	int createRef = LUA_NOREF;
	int unloadRef = LUA_NOREF;
};
