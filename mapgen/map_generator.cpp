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

#include "maths.hpp"

#include <cmath>

//it's either this, or dealing with floating points
struct Coord {
	int x, y;
};

//std::abs() only uses floating points
int absolute(int i) {
	return i > 0 ? i : -i;
}

//calculate how far away a query point is from a grid point
Coord calcDist(Coord gridPoint, Coord queryPoint) {
	int i = absolute(queryPoint.x - gridPoint.x);
	int j = absolute(queryPoint.y - gridPoint.y);
	return {i, j};
}

//calculate the amount of influence a distance will have within a certain sized area
int calcInfluence(Coord dist, int width, int height) {
	//inverted distance
	Coord i = {width - dist.x, height - dist.y};
	//essentially calculating the length
	return sqrt(i.x*i.x + i.y*i.y);
}

//determine the scaled value from the grid point
int calcScale(int influence, int gridValue, int pointValue) {
	/* TODO
	 * (influence / {width, height}.Length()) //percentage to interpolate
	 * poingValue - gridValue //difference
	 * return percentage * difference //scale
	*/
	return gridValue;
}

int MapGenerator::Noise(int x, int y, int width, int height) {
	Coord queryPoint = {x, y};

	//the "grid points"
	Coord tl = {snap(x, width), snap(y, height)};
	Coord tr = {tl.x + width, tl.y};
	Coord bl = {tl.x, tl.y + height};
	Coord br = {tl.x + width, tl.y + height};

	//get the distance from the query, and subsequently the influence
	int tlInfluence = calcInfluence(calcDist(tl, queryPoint), width, height);
	int trInfluence = calcInfluence(calcDist(tr, queryPoint), width, height);
	int blInfluence = calcInfluence(calcDist(bl, queryPoint), width, height);
	int brInfluence = calcInfluence(calcDist(br, queryPoint), width, height);

	//now combine the grid's values with the influence values
	int tlScale = calcScale(tlInfluence, RawNoise(tl.x, tl.y), RawNoise(queryPoint.x, queryPoint.y));
	int trScale = calcScale(trInfluence, RawNoise(tr.x, tr.y), RawNoise(queryPoint.x, queryPoint.y));
	int blScale = calcScale(blInfluence, RawNoise(bl.x, bl.y), RawNoise(queryPoint.x, queryPoint.y));
	int brScale = calcScale(brInfluence, RawNoise(br.x, br.y), RawNoise(queryPoint.x, queryPoint.y));

	//Finally, apply each scale to the raw value, resulting in the height map
	return tlScale + trScale + blScale + brScale + RawNoise(queryPoint.x, queryPoint.y);
}

int MapGenerator::RawNoise(int x, int y) {
	return (x * 11235 + y * 81321 + 3455) % 256;
}