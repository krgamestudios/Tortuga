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
#ifndef REGION_HPP_
#define REGION_HPP_

#define REGION_WIDTH 20
#define REGION_HEIGHT 20
#define REGION_DEPTH 3

class Region {
public:
	typedef unsigned short type_t;

	Region() = delete;
	Region(int x, int y);
	~Region() = default;

	type_t SetTile(int x, int y, int z, type_t v);
	type_t GetTile(int x, int y, int z);

	//accessors
	int GetX() const { return x; }
	int GetY() const { return y; }
private:
	const int x;
	const int y;

	type_t tiles[REGION_WIDTH][REGION_HEIGHT][REGION_DEPTH];
};

#endif
