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
#include "base_monster.hpp"

#include "config_utility.hpp"

void BaseMonster::CorrectSprite() {
	//TODO: (9) BaseMonster::CorrectSprite()
}

std::string BaseMonster::SetHandle(std::string s) {
	return handle = s;
}

std::string BaseMonster::GetHandle() const {
	return handle;
}

std::string BaseMonster::SetAvatar(std::string s) {
	avatar = s;
	sprite.LoadSurface(ConfigUtility::GetSingleton()["dir.sprites"] + avatar, 4, 1);
	return avatar;
}

std::string BaseMonster::GetAvatar() const {
	return avatar;
}