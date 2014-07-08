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
#include "vector2.hpp"
#include "bounding_box.hpp"

//graphics
#include "sprite_sheet.hpp"

//std namespace
#include <cmath>

//the speeds that the characters move
constexpr double CHARACTER_WALKING_SPEED = 140.0;
constexpr double CHARACTER_WALKING_MOD = 1.0/sqrt(2.0);

class Character {
public:
	Character() = default;
	~Character() = default;

	void Update(double delta);

	//graphics
	void DrawTo(SDL_Surface* const, int camX, int camY);
	void CorrectSprite();
	SpriteSheet* GetSprite() { return &sprite; }

	//accessors and mutators
	Vector2 SetOrigin(Vector2 v) { return origin = v; }
	Vector2 GetOrigin() const { return origin; }
	Vector2 SetMotion(Vector2 v) { return motion = v; }
	Vector2 GetMotion() const { return motion; }
	BoundingBox SetBoundingBox(BoundingBox b) { return bounds = b; }
	BoundingBox GetBoundingBox() const { return bounds; }

private:
	//graphics
	SpriteSheet sprite;

	//position
	Vector2 origin = {0.0,0.0};
	Vector2 motion = {0.0,0.0};
	BoundingBox bounds;
};

#endif
