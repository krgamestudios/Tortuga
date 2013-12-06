/* Copyright: (c) Kayne Ruse 2013
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
#ifndef VECTOR2_HPP_
#define VECTOR2_HPP_

#include <stdexcept>
#include <cmath>

class Vector2 {
public:
	double x, y;

	//This is explicitly a POD
	Vector2() = default;
	Vector2(double i, double j): x(i), y(j) {};
	~Vector2() = default;
	Vector2& operator=(Vector2 const&) = default;

	double Length() const {
		return sqrt(x*x+y*y);
	}
	double SquaredLength() const {
		return x*x+y*y;
	}

	double operator[](size_t i) {
		if (i >= 2)
			throw(std::domain_error("Out of range"));
		return *(&x+i);
	}

	//Arithmetic operators
	Vector2 operator+(Vector2 v) const {
		Vector2 ret;
		ret.x = x + v.x;
		ret.y = y + v.y;
		return ret;
	}
	Vector2 operator-(Vector2 v) const {
		Vector2 ret;
		ret.x = x - v.x;
		ret.y = y - v.y;
		return ret;
	}
	Vector2 operator*(Vector2 v) const {
		Vector2 ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		return ret;
	}
	Vector2 operator*(double d) const {
		Vector2 ret;
		ret.x = x * d;
		ret.y = y * d;
		return ret;
	}

	Vector2 operator/(Vector2 v) {
		if (!v.x || !v.y)
			throw(std::domain_error("Divide by zero"));
		Vector2 ret;
		ret.x = x / v.x;
		ret.y = y / v.y;
		return ret;
	}
	Vector2 operator/(double d) {
		if (!d)
			throw(std::domain_error("Divide by zero"));
		Vector2 ret;
		ret.x = x / d;
		ret.y = y / d;
		return ret;
	}

	bool operator==(Vector2 v) { return (x == v.x && y == v.y); }
	bool operator!=(Vector2 v) { return (x != v.x || y != v.y); }

	//member templates (curry the above operators)
	template<typename T> Vector2 operator+=(T t) { return *this = *this + t; }
	template<typename T> Vector2 operator-=(T t) { return *this = *this - t; }
	template<typename T> Vector2 operator*=(T t) { return *this = *this * t; }
	template<typename T> Vector2 operator/=(T t) { return *this = *this / t; }
	template<typename T> bool operator==(T t) { return (x == t && y == t); }
	template<typename T> bool operator!=(T t) { return (x != t || y != t); }
};

//non-member templates (flip the order)
template<typename T> Vector2 operator+(T t, Vector2 v) { return v + t; }
template<typename T> Vector2 operator-(T t, Vector2 v) { return v - t; }
template<typename T> Vector2 operator*(T t, Vector2 v) { return v * t; }
template<typename T> Vector2 operator/(T t, Vector2 v) { return v / t; }

template<typename T> bool operator==(T t, Vector2 v) { return v == t; }
template<typename T> bool operator!=(T t, Vector2 v) { return v != t; }

#endif
