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

#include "entity.hpp"

#include "lua.hpp"

#include <map>
#include <string>

class BarrierData: public Entity {
public:
	BarrierData(int instanceIndex);
	~BarrierData();

	int Update(lua_State*);

	int SetScriptReference(int);
	int GetScriptReference();

	std::string SetTag(std::string key, std::string value);
	std::string GetTag(std::string key);

	int SetInstanceIndex(int i);
	int GetInstanceIndex() const;

private:
	int scriptRef = LUA_NOREF;
	std::map<std::string, std::string> tags;

	int instanceIndex = -1;
};