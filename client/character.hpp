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
#ifndef CHARACTER_HPP_
#define CHARACTER_HPP_

//components
#include "character_defines.hpp"
#include "vector2.hpp"
#include "bounding_box.hpp"
#include "statistics.hpp"

//graphics
#include "sprite_sheet.hpp"

//std namespace
#include <string>
#include <cmath>

class Character {
public:
	Character() = default;
	~Character() = default;

	void Update();

	//graphics
	void DrawTo(SDL_Surface* const, int camX, int camY);
	void CorrectSprite();
	SpriteSheet* GetSprite() { return &sprite; }

	//gameplay
	Statistics* GetStats() { return &stats; }

	//accessors and mutators

	//metadata
	int SetOwner(int i) { return owner = i; }
	int GetOwner() { return owner; }
	std::string SetHandle(std::string s) { return handle = s; }
	std::string GetHandle() const { return handle; }
	std::string SetAvatar(std::string s) { return avatar = s; }
	std::string GetAvatar() const { return avatar; }

	//position
	Vector2 SetOrigin(Vector2 v) { return origin = v; }
	Vector2 GetOrigin() const { return origin; }
	Vector2 SetMotion(Vector2 v) { return motion = v; }
	Vector2 GetMotion() const { return motion; }
	BoundingBox SetBounds(BoundingBox b) { return bounds = b; }
	BoundingBox GetBounds() { return bounds; }

private:
	//graphics
	SpriteSheet sprite;

	//base statistics
	Statistics stats;

	//gameplay components: equipment, items, buffs, debuffs...

	//metadata
	int owner;
	std::string handle;
	std::string avatar;

	//position
	Vector2 origin = {0.0,0.0};
	Vector2 motion = {0.0,0.0};
	BoundingBox bounds;
};

//tmp
#include <map>
typedef std::map<int, Character> CharacterMap;

#endif
