/* Copyright: (c) Kayne Ruse 2013, 2014
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
#ifndef BBOX_HPP_
#define BBOX_HPP_

#include <type_traits>
#include <stdexcept>
#include <algorithm>

//TODO: This is supposed to interact with the vector
class BBox {
public:
	double x, y;
	double w, h;

	BBox() = default;
	BBox(double i, double j, double k, double l): x(i), y(j), w(k), h(l) {};
	~BBox() = default;
	BBox& operator=(BBox const&) = default;

	double Size() {
		return std::max(w*h,0.0);
	}

	bool IsCollision(BBox rhs) {
		return not (
			x >= rhs.x + rhs.w ||
			y >= rhs.y + rhs.h ||
			rhs.x >= x + w ||
			rhs.y >= y + h
		);
	}

	BBox Intersection(BBox rhs) {
		if (!IsCollision(rhs)) {
			return {0, 0, 0, 0};
		}
		BBox ret;
		ret.x = std::max(x, rhs.x);
		ret.y = std::max(y, rhs.y);
		ret.w = std::min(x+w, rhs.x+rhs.w) - ret.x;
		ret.h = std::min(y+h, rhs.y+rhs.h) - ret.y;
		return ret;
	}

	double operator[](size_t i) {
		if (i >= 4)
			throw(std::domain_error("Out of range"));
		return *(&x+i);
	}
};

//This is explicitly a POD
static_assert(std::is_pod<BBox>::value, "BBox is not a POD");

#endif