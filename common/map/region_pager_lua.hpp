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
#ifndef REGIONPAGERLUA_HPP_
#define REGIONPAGERLUA_HPP_

#include "region_pager_base.hpp"

#include "lua/lua.hpp"

#include <string>

class RegionPagerLua : public RegionPagerBase {
public:
	RegionPagerLua() = default;
	~RegionPagerLua() = default;

	//region manipulation
	Region* LoadRegion(int x, int y) override;
	Region* SaveRegion(int x, int y) override;
	Region* CreateRegion(int x, int y) override;
	void UnloadRegion(int x, int y) override;

	void UnloadAll() override;

	//accessors & mutators
	std::string SetDirectory(std::string s) { return directory = s; }
	std::string GetDirectory() { return directory; }

	lua_State* SetLuaState(lua_State* L) { return luaState = L; }
	lua_State* GetLuaState() { return luaState; }
protected:
	std::string directory;
	lua_State* luaState = nullptr;
};

#endif
