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
#ifndef BOUNDINGBOX_HPP_
#define BOUNDINGBOX_HPP_

#include <type_traits>
#include <algorithm>

class BoundingBox {
public:
	//This is explicitly a POD
	int x, y;
	int w, h;

	BoundingBox() = default;
	BoundingBox(int i, int j): x(i), y(j), w(0), h(0) {};
	BoundingBox(int i, int j, int k, int l): x(i), y(j), w(k), h(l) {};
	~BoundingBox() = default;
	BoundingBox& operator=(BoundingBox const&) = default;

	int Size() {
		return std::max(w*h,0);
	}

	bool CheckOverlap(BoundingBox rhs) {
		return !(
			x >= rhs.x + rhs.w ||
			y >= rhs.y + rhs.h ||
			rhs.x >= x + w ||
			rhs.y >= y + h);
	}

	BoundingBox CalcOverlap(BoundingBox rhs) {
		if (!CheckOverlap(rhs)) {
			return {0, 0, 0, 0};
		}
		BoundingBox ret;
		ret.x = std::max(x, rhs.x);
		ret.y = std::max(y, rhs.y);
		ret.w = std::min(x+w, rhs.x+rhs.w) - ret.x;
		ret.h = std::min(y+h, rhs.y+rhs.h) - ret.y;
		return ret;
	}
};

//This is explicitly a POD
static_assert(std::is_pod<BoundingBox>::value, "BoundingBox is not a POD");

#include "vector2.hpp"

//operators
inline BoundingBox operator+(BoundingBox b, Vector2 v) {
	return {b.x + (int)v.x, b.y + (int)v.y, b.w, b.h};
}
inline BoundingBox operator+(Vector2 v, BoundingBox b) {
	return b + v;
}

#endif
