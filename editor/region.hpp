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
#ifndef REGION_HPP_
#define REGION_HPP_

#include "tile.hpp"

#include <set>

/* A single section of the map.
 * This class stores the tiles relative to it's own position, but
 * there are functions for referencing the tiles' absolute position.
 * These functions simply wrap the normal functions.
*/
class Region {
public:
	Region() = delete;
	Region(int x, int y, int width, int height);
	~Region() = default;

	//create and insert a new tile, overwriting an existing tile at that location
	bool NewTileR(Tile const& tile);
	bool NewTileA(Tile const& tile) {
		//these can change, if the Tile class is changed
		return NewTileR({
			tile.x - x,
			tile.y - y,
			tile.depth,
			tile.width,
			tile.height,
			tile.sheetIndex,
			tile.tileIndex
		});
	}

	//find the first tile at this location, with the specified minimum depth
	Tile GetTileR(int tx, int ty, int minDepth);
	Tile GetTileA(int tx, int ty, int minDepth) {
		return GetTileR(tx - x, ty - y, minDepth);
	}

	//wrap the other delete functions
	bool DeleteTileR(int tx, int ty, int minDepth) {
		return DeleteTileR(GetTileR(tx, ty, minDepth));
	}
	bool DeleteTileA(int tx, int ty, int minDepth) {
		//explicitly skip one function call by adjusting from A to R
		return DeleteTileR(GetTileR(tx - x, ty - y, minDepth));
	}

	//delete the specified tile
	bool DeleteTileR(Tile const& tile);
	bool DeleteTileA(Tile const& tile) {
		//these can change, if the Tile class is changed
		return DeleteTileR({
			tile.x - x,
			tile.y - y,
			tile.depth,
			tile.width,
			tile.height,
			tile.sheetIndex,
			tile.tileIndex
		});
	}

	//find if the specified location exists within the region's bounds
	bool InBoundsR(int i, int j) {
		return (i >= 0) && (j >= 0) && (i < width) && (j < height);
	}
	bool InBoundsA(int i, int j) {
		return InBoundsR(i - x, j - y);
	}

	//Raw accessors & mutators
	int GetX() const { return x; }
	int GetY() const { return y; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	std::set<Tile>* GetTiles() { return &tiles; }

	//sorting the regions by the locations
	friend bool operator<(Region const& lhs, Region const& rhs);
	friend bool operator>(Region const& lhs, Region const& rhs);
	friend bool operator==(Region const& lhs, Region const& rhs);

private:
	int const x;
	int const y;
	int const width;
	int const height;
	std::set<Tile> tiles;
};

#endif
