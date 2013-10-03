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
#ifndef TILE_HPP_
#define TILE_HPP_

//explicitly a POD
struct Tile {
	//position relative to the Region
	int x, y, depth;

	//graphics
	int width, height;
	int tileIndex;

	Tile() = default;
	Tile(int _x, int _y, int _depth, int _width, int _height, int _tileIndex) {
		//The order of the arguments should be explicit
		x = _x;
		y = _y;
		depth = _depth;
		width = _width;
		height = _height;
		tileIndex = _tileIndex;
	}
};

bool operator<(Tile const& lhs, Tile const& rhs);
bool operator>(Tile const& lhs, Tile const& rhs);
bool operator==(Tile const& lhs, Tile const& rhs);

#endif
