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
#include "monster_manager.hpp"

//TODO: monster_manager.cpp

int MonsterManager::Create(std::string) {
	//
}

int MonsterManager::Load(std::string) {
	//
}

int MonsterManager::Save(int uid) {
	//
}

void MonsterManager::Unload(int uid) {
	//
}

void MonsterManager::Delete(int uid) {
	//
}

void MonsterManager::UnloadAll() {
	//
}

void MonsterManager::UnloadIf(std::function<bool(std::pair<const int, MonsterData>)> fn) {
	//
}

MonsterData* MonsterManager::Get(int uid) {
	//
}

int MonsterManager::GetLoadedCount() {
	//
}

int MonsterManager::GetTotalCount() {
	//
}

std::map<int, MonsterData>* MonsterManager::GetContainer() {
	//
}

sqlite3* MonsterManager::SetDatabase(sqlite3* db) {
	//
}

sqlite3* MonsterManager::GetDatabase() {
	//
}

lua_State* MonsterManager::SetLuaState(lua_State* L) {
	//
}

lua_State* MonsterManager::GetLuaState() {
	//
}
