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
#include "monster_data.hpp"

MonsterData::MonsterData(std::string _avatar, int _scriptRef):
	Entity("monster"),
	avatar(_avatar),
	scriptRef(_scriptRef)
{
	//EMPTY
}

void MonsterData::Update() {
	Entity::Update();
	//TODO: (0) call the script reference
}

//-------------------------
//accessors & mutators
//-------------------------

std::string MonsterData::SetAvatar(std::string s) {
	return avatar = s;
}

std::string MonsterData::GetAvatar() {
	return avatar;
}

int MonsterData::SetScriptReference(int i) {
	return scriptRef = i;
}

int MonsterData::GetScriptReference() {
	return scriptRef;
}