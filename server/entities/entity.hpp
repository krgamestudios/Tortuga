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
#pragma once

#include "bounding_box.hpp"
#include "vector2.hpp"

#include <string>

//The base class for all objects in the world
class Entity {
public:
	virtual void Update();

	//accessors & mutators
	int SetRoomIndex(int i);
	Vector2 SetOrigin(Vector2 v);
	Vector2 SetMotion(Vector2 v);
	BoundingBox SetBounds(BoundingBox b);

	int GetRoomIndex() const;
	Vector2 GetOrigin() const;
	Vector2 GetMotion() const;
	BoundingBox GetBounds() const;

	const char* GetType() const;

protected:
	Entity(const char* type);
	virtual ~Entity() = default;

	int roomIndex = -1;
	Vector2 origin = {0, 0};
	Vector2 motion = {0, 0};
	BoundingBox bounds = {0, 0, 0, 0};
	const char* type;
};
