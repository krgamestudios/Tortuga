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
#ifndef MAPGENERATOR_HPP_
#define MAPGENERATOR_HPP_

#include "simple_rng.hpp"

#include "vector2.hpp"

class MapGenerator {
public:
	MapGenerator() = default;
	~MapGenerator() = default;

	Vector2 RawNoise(Vector2 const&);
	double Influence(Vector2 const& gridPoint, Vector2 const& queryPoint, double width, double height);
	double ScaledNoise(double x, double y, double width, double height);
	double ScaledOctave(double x, double y, double width, double height, double octave);
	double GetPixel(double x, double y, double width, double height, double octave);

private:
	SimpleRNG rng;
};

#endif