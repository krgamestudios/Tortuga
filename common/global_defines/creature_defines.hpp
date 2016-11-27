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
#pragma once

#include <cmath>

//the speeds that the characters move
constexpr double CREATURE_WALKING_SPEED = 1.0;
constexpr double CREATURE_WALKING_MOD = 1.0/sqrt(2.0);
constexpr double CREATURE_WALKING_NEGATIVE_MOD = 1.0 - CREATURE_WALKING_MOD;

//the bounds for the character objects, mapped to the default sprites
constexpr int CREATURE_BOUNDS_X = 0;
constexpr int CREATURE_BOUNDS_Y = 0;
constexpr int CREATURE_BOUNDS_WIDTH = 32;
constexpr int CREATURE_BOUNDS_HEIGHT = 32;

//the character's sprite format
constexpr int CREATURE_CELLS_X = 4;
constexpr int CREATURE_CELLS_Y = 4;
