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
#include "barrier_data.hpp"

#include <cstring>
#include <sstream>

BarrierData::BarrierData(int i):
	Entity::Entity("barrier")
{
	instanceIndex = i;
	memset(status, 0, sizeof(int) * 8);
}

BarrierData::~BarrierData() {
	//
}

int BarrierData::Update(lua_State* L) {
	int ret = 0;

	if (scriptRef != LUA_NOREF) {
		//Call the script reference
		lua_pushinteger(L, scriptRef);
		lua_gettable(L, LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L, reinterpret_cast<void*>(this));

		//check for errors
		if(lua_pcall(L, 1, 1, 0) != LUA_OK) {
			std::ostringstream msg;
			msg << "Error running creature script: " << lua_tostring(L, -1);
			lua_pop(L, 1);
			throw(std::runtime_error(msg.str()));
		}

		ret += lua_tonumber(L, -1);
	}

	Entity::Update();

	return ret;
}

int BarrierData::SetScriptReference(int i) {
	return scriptRef = i;
}

int BarrierData::GetScriptReference() {
	return scriptRef;
}

std::string BarrierData::SetTag(std::string key, std::string value) {
	return tags[key] = value;
}

std::string BarrierData::GetTag(std::string key) {
	return tags[key];
}

int BarrierData::SetInstanceIndex(int i) {
	return instanceIndex = i;
}

int BarrierData::GetInstanceIndex() const {
	return instanceIndex;
}

int BarrierData::SetStatus(int k, int v) {
	if (k < 0 || k >= 8) {
		return -1;
	}
	return status[k] = v;
}

int BarrierData::GetStatus(int k) {
	if (k < 0 || k >= 8) {
		return -1;
	}
	return status[k];
}

int* BarrierData::GetStatusArray() {
	return status;
}