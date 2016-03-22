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
#include "base_creature.hpp"

#include "config_utility.hpp"

void BaseCreature::CorrectSprite() {
	//TODO: (9) BaseCreature::CorrectSprite()
}

std::string BaseCreature::SetHandle(std::string s) {
	return handle = s;
}

std::string BaseCreature::GetHandle() const {
	return handle;
}

std::string BaseCreature::SetAvatar(SDL_Renderer* const renderer, std::string s) {
	avatar = s;
	sprite.Load(renderer, ConfigUtility::GetSingleton()["dir.sprites"] + avatar, 4, 1);
	return avatar;
}

std::string BaseCreature::GetAvatar() const {
	return avatar;
}