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
#include "perlin_noise.hpp"

#include "vector2.hpp"

#include <cmath>

//-------------------------
//Public access members
//-------------------------

double SimpleRNG::operator()(double x) {
	x = x * seed * 345589144.0 + 1123581321.0;
	return fmod(x, 21795.0);
};

Vector2 PerlinNoise::Gradient(Vector2 pos) {
	//could cache this result
	double j = rng(pos.y*pos.y + pos.x * 144);
	double i = rng(pos.x*pos.x + pos.y * 144);
	Vector2 v(i, j);
	v.Normalize();
	return v;
}

double PerlinNoise::Influcence(Vector2 gridPoint, Vector2 queryPoint) {
	return ScalarProduct(Gradient(gridPoint), queryPoint - gridPoint);
}

double PerlinNoise::Noise(double x, double y, double width, double height) {
	Vector2 point = {x, y};

	x = Snap(x, width);
	y = Snap(y, height);

	//determine the grid points
	Vector2 tl = {x, y};
	Vector2 tr = {x + width, y};
	Vector2 bl = {x, y + height};
	Vector2 br = {x + width, y + height};

	//influence equasion
	double s = Influcence(tl, point);
	double t = Influcence(tr, point);
	double u = Influcence(bl, point);
	double v = Influcence(br, point);

	//get the noise
	double a = s + Curve((t - s) / width);
	double b = u + Curve((v - u) / width);
	return a + Curve((b - a) / height);
}

//-------------------------
//Static utility functions
//-------------------------

double PerlinNoise::Curve(double x) {
	//param: 0 to 1 inclusive
	return 3.0 * pow(x, 2.0) - 2.0 * pow(x, 3.0);
}

double PerlinNoise::Snap(double x, double base) {
	//snap to a grid (floating point version)
	return floor(x / base) * base;
}

double PerlinNoise::ScalarProduct(Vector2 lhs, Vector2 rhs) {
	//the dot product
	return lhs.x * rhs.x + lhs.y * rhs.y;
}