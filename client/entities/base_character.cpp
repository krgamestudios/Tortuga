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
#include "base_character.hpp"

#include "config_utility.hpp"

//-------------------------
//graphics
//-------------------------

void BaseCharacter::CorrectSprite() {
	//NOTE: These must correspond to the sprite sheet in use
	if (motion.y > 0) {
		sprite.SetYIndex(0);
	}
	else if (motion.y < 0) {
		sprite.SetYIndex(1);
	}
	else if (motion.x > 0) {
		sprite.SetYIndex(3);
	}
	else if (motion.x < 0) {
		sprite.SetYIndex(2);
	}

	//animation
	if (motion != 0) {
		sprite.SetDelay(0.1);
	}
	else {
		sprite.SetDelay(0);
		sprite.SetXIndex(0);
	}
}

//-------------------------
//metadata
//-------------------------

int BaseCharacter::SetOwner(int i) {
	return owner = i;
}

int BaseCharacter::GetOwner() {
	return owner;
}

std::string BaseCharacter::SetHandle(std::string s) {
	return handle = s;
}

std::string BaseCharacter::GetHandle() const {
	return handle;
}

std::string BaseCharacter::SetAvatar(std::string s) {
	avatar = s;
	sprite.LoadSurface(ConfigUtility::GetSingleton()["dir.sprites"] + avatar, CHARACTER_CELLS_X, CHARACTER_CELLS_Y);
	return avatar;
}

std::string BaseCharacter::GetAvatar() const {
	return avatar;
}