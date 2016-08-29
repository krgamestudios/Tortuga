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
#include "character_data.hpp"

CharacterData::CharacterData(): Entity("character") {
	SetBounds({
		CHARACTER_BOUNDS_X,
		CHARACTER_BOUNDS_Y,
		CHARACTER_BOUNDS_WIDTH,
		CHARACTER_BOUNDS_HEIGHT
	});
}

//-------------------------
//database stuff
//-------------------------

int CharacterData::GetIndex() {
	return index;
}

int CharacterData::GetOwner() {
	return owner;
}

std::string CharacterData::GetHandle() {
	return handle;
}

std::string CharacterData::GetAvatar() {
	return avatar;
}

Inventory* CharacterData::GetInventory() {
	return &inventory;
}