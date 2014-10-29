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
#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include "vector2.hpp"

//The base class for all objects in the world
class Entity {
public:
	//accessors & mutators
	int SetEntityIndex(int i)		{ return entityIndex = i; }
	int SetRoomIndex(int i)			{ return roomIndex = i; }
	Vector2 SetOrigin(Vector2 v)	{ return origin = v; }
	Vector2 SetMotion(Vector2 v)	{ return motion = v; }

	int GetEntityIndex()	{ return entityIndex; }
	int GetRoomIndex()		{ return roomIndex; }
	Vector2 GetOrigin()		{ return origin; }
	Vector2 GetMotion()		{ return motion; }

protected:
	Entity() = default;
	~Entity() = default;

	int entityIndex = -1;
	int roomIndex = -1;
	Vector2 origin;
	Vector2 motion;
};

#endif