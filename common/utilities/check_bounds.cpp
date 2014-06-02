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
#include "check_bounds.hpp"

bool checkPoint(Vector2 const& origin, Vector2 const& bound, Vector2 const& point) {
	return !(
		point.x < origin.x ||
		point.y < origin.y ||
		point.x >= origin.x + bound.x ||
		point.y >= origin.y + bound.y
	);
}

bool checkOverlap(Vector2 const& originOne, Vector2 const& boundOne, Vector2 const& originTwo, Vector2 const& boundTwo) {
	return !(
		originOne.x >= originTwo.x + boundTwo.x ||
		originOne.x + boundOne.x >= originTwo.x ||
		originOne.y >= originTwo.y + boundTwo.y ||
		originOne.y + boundOne.y >= originTwo.y
	);
}
