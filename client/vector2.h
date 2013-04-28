#ifndef VECTOR2_H_
#define VECTOR2_H_

#include <stdexcept>
#include <cmath>

class Vector2 {
public:
	/* Public access members */
	double x, y;

	inline Vector2():
	x(0), y(0)
	{
	}

	inline Vector2(double d):
	x(d), y(d)
	{
	}

	inline Vector2(double _x, double _y):
	x(_x), y(_y)
	{
	}

	inline double Length() {
		return sqrt(x*x+y*y);
	}

	inline double SquaredLength()  {
		return x*x+y*y;
	}

	inline double operator[](size_t i) {
		if (i >= 2)
			throw(std::domain_error("Out of range"));

		return *(&x+i);
	}

	/* Arithmetic operators */
	inline Vector2 operator+(Vector2 v) { return Vector2(x + v.x, y + v.y); }
	inline Vector2 operator-(Vector2 v) { return Vector2(x - v.x, y - v.y); }
	inline Vector2 operator*(Vector2 v) { return Vector2(x * v.x, y * v.y); }
	inline Vector2 operator*(double d) { return Vector2(x * d, y * d); }

	inline Vector2 operator/(Vector2 v) {
		if (!v.x || !v.y)
			throw(std::domain_error("Divide by zero"));

		return Vector2(x / v.x, y / v.y);
	}

	inline Vector2 operator/(double d) {
		if (!d)
			throw(std::domain_error("Divide by zero"));

		return Vector2(x / d, y / d);
	}

	template<typename T> inline Vector2 operator+=(T t) { return *this = *this + t; }
	template<typename T> inline Vector2 operator-=(T t) { return *this = *this - t; }
	template<typename T> inline Vector2 operator*=(T t) { return *this = *this * t; }
	template<typename T> inline Vector2 operator/=(T t) { return *this = *this / t; }

	inline bool operator==(Vector2 v) { return (x == v.x && y == v.y); }
	inline bool operator!=(Vector2 v) { return (x != v.x || y != v.y); }
	template<typename T> inline bool operator==(T t) { return (x == t && y == t); }
	template<typename T> inline bool operator!=(T t) { return (x != t || y != t); }
};

#endif
