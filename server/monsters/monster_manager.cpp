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
#include "monster_manager.hpp"

int MonsterManager::Create(std::string) {
	//TODO
}

int MonsterManager::Load(std::string) {
	//TODO
}

int MonsterManager::Save(int uid) {
	//TODO
}

void MonsterManager::Unload(int uid) {
	//TODO
}

void MonsterManager::Delete(int uid) {
	//TODO
}

void MonsterManager::UnloadAll() {
	//TODO
}

void MonsterManager::UnloadIf(std::function<bool(std::pair<const int, MonsterData>)> fn) {
	//TODO
}

MonsterData* MonsterManager::Get(int uid) {
	//TODO
}

int MonsterManager::GetLoadedCount() {
	//TODO
}

int MonsterManager::GetTotalCount() {
	//TODO
}

std::map<int, MonsterData>* MonsterManager::GetContainer() {
	//TODO
}

sqlite3* MonsterManager::SetDatabase(sqlite3* db) {
	//TODO
}

sqlite3* MonsterManager::GetDatabase() {
	//TODO
}

lua_State* MonsterManager::SetLuaState(lua_State* L) {
	//TODO
}

lua_State* MonsterManager::GetLuaState() {
	//TODO
}
