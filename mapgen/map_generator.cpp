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
#include "map_generator.hpp"

#include "vector2.hpp"

#include <cmath>

//-------------------------
//Utility functions
//-------------------------

//snap to a grid (floating point version)
static double snap(double x, double base) {
	return floor(x / base) * base;
}

//A.K.A.: the dot product
static double scalarProduct(Vector2 lhs, Vector2 rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

//curved interpolation
static double curve(double x) {
	//param: 0 to 1 inclusive
	return 3.0 * pow(x, 2.0) - 2.0 * pow(x, 3.0);
}

//fix the overflow
static double curl(double x) {
	if (x > 1.0) {
		return -curl(x-1) +1;
	}
	if (x < 0.0) {
		return curl(-x);
	}
	return x;
}

static double cut(double x) {
	if (x > 1.0) {
		return 1.0;
	}
	if (x < 0.0) {
		return 0.0;
	}
	return x;
}

//-------------------------
//Public methods
//-------------------------

Vector2 MapGenerator::RawNoise(Vector2 const& gridPoint) {
	double angle = rng(gridPoint.x * 0xffff + gridPoint.y);
	Vector2 v = {cos(angle), sin(angle)};
	v.Normalize();
	return v;
}

double MapGenerator::Influence(Vector2 const& gridPoint, Vector2 const& queryPoint, double width, double height) {
	//note: inverting the distance here, so the smaller the distance the more influence it has
	Vector2 distance = queryPoint - gridPoint;
	Vector2 inverted = {width - distance.x, height - distance.y};
	double ret = scalarProduct(RawNoise(gridPoint), inverted);
	return ret > 0 ? ret : -ret;
}

double MapGenerator::ScaledNoise(double x, double y, double width, double height) {
	Vector2 queryPoint = {x, y};

	//the "grid points"
	Vector2 tl = {snap(x, width), snap(y, height)};
	Vector2 tr = {tl.x + width, tl.y};
	Vector2 bl = {tl.x, tl.y + height};
	Vector2 br = {tl.x + width, tl.y + height};

	//influence equasion
	double s = Influence(tl, queryPoint, width, height);
	double t = Influence(tr, queryPoint, width, height);
	double u = Influence(bl, queryPoint, width, height);
	double v = Influence(br, queryPoint, width, height);

	//Finally, calc the value
	double a = s + curve((t - s) / width);
	double b = u + curve((v - u) / width);
	return curve((b - a) / height);
}

double MapGenerator::ScaledOctave(double x, double y, double width, double height, double octave) {
	double ret = 0;
	if (octave > 1) {
		ret += ScaledOctave(x, y, width/2, height/2, octave-1);
	}
	return ret / octave + ScaledNoise(x, y, width, height);
}

double MapGenerator::GetPixel(double x, double y, double width, double height, double octave) {
	//use this as a decorator function
	return curl(ScaledOctave(x, y, width, height, octave));
}