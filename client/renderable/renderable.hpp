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
#ifndef RENDERABLE_HPP_
#define RENDERABLE_HPP_

#include "bounding_box.hpp"
#include "sprite_sheet.hpp"
#include "vector2.hpp"

class Renderable {
public:
	Renderable() = default;
	virtual ~Renderable() = default;

	virtual void Update();
	virtual void DrawTo(SDL_Surface* const, int camX, int camY);

	SpriteSheet* GetSprite() { return &sprite; }

	//position
	Vector2 SetOrigin(Vector2 v) { return origin = v; }
	Vector2 GetOrigin() const { return origin; }
	Vector2 SetMotion(Vector2 v) { return motion = v; }
	Vector2 GetMotion() const { return motion; }

	//collision
	BoundingBox SetBounds(BoundingBox b) { return bounds = b; }
	BoundingBox GetBounds() { return bounds; }

protected: //TODO: should be private
	SpriteSheet sprite;
	Vector2 origin = {0, 0};
	Vector2 motion = {0, 0};
	BoundingBox bounds;
};

#endif