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
#ifndef PERLINNOISE_HPP_
#define PERLINNOISE_HPP_

#include "vector2.hpp"

class SimpleRNG {
public:
	SimpleRNG() = default;
	SimpleRNG(double x) { SetSeed(x); }

	double SetSeed(double s) { return seed = s; }
	double GetSeed() { return seed; }

	double operator()(double x);

private:
	double seed = 8891.0;
};

class PerlinNoise {
public:
	PerlinNoise() = default;
	~PerlinNoise() = default;

	Vector2 Gradient(Vector2);
	double Influcence(Vector2 gridPoint, Vector2 queryPoint);

	double Noise(double x, double y, double width, double height);
private:
	static inline double Curve(double x);
	static inline double Snap(double x, double base);
	static inline double ScalarProduct(Vector2 lhs, Vector2 rhs);

	SimpleRNG rng;
};

#endif
