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
#include "entity.hpp"

void Entity::Update() {
	origin += motion;
	sprite.Update(0.016);
}

void Entity::DrawTo(SDL_Renderer* const renderer, int camX, int camY) {
	sprite.DrawTo(renderer, origin.x - camX, origin.y - camY);
}

SpriteSheet* Entity::GetSprite() {
	return &sprite;
}

//-------------------------
//accessors & mutators
//-------------------------

Vector2 Entity::SetOrigin(Vector2 v) {
	return origin = v;
}

Vector2 Entity::SetMotion(Vector2 v) {
	return motion = v;
}

BoundingBox Entity::SetBounds(BoundingBox b) {
	return bounds = b;
}

Vector2 Entity::GetOrigin() {
	return origin;
}

Vector2 Entity::GetMotion() {
	return motion;
}

BoundingBox Entity::GetBounds() {
	return bounds;
}