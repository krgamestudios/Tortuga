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
#include "region.hpp"

#include "utility.hpp"

#include <stdexcept>
#include <sstream>

Region::Region(int _x, int _y, int _w, int _h):
	x(_x),
	y(_y),
	width(_w),
	height(_h)
{
	//make sure that the region's position lines up
	if (x != snapToBase(width, x) || y != snapToBase(height, y)) {
		std::ostringstream os;
		os << "Region is unaligned; x: " << x << ", y: " << y << ", width: " << width << ", height: " << height;
		throw(std::runtime_error(os.str()));
	}
}

int Region::NewTileR(Tile const& tile) {
	//return 1 for overwrite, 0 for insert
	if (!InBoundsR(tile.x, tile.y)) {
		throw(std::runtime_error("New tile location out of bounds"));
	}

	int ret = tiles.erase(tile);
	tiles.insert(tile);
	return ret;
}

Tile Region::GetTileR(int tx, int ty, int minDepth) {
	std::set<Tile>::iterator ptr = tiles.begin();

	//skip the tiles that are too deep
	while(ptr != tiles.end()) {
		if (ptr->depth >= minDepth) {
			break;
		}
		ptr++;
	}

	//find the first tile here
	while(ptr != tiles.end()) {
		//bounds
		if ((ptr->x <= tx) && (ptr->y <= ty) && (ptr->x + ptr->width > tx) && (ptr->y + ptr->height > ty)) {
			break;
		}
		ptr++;
	}

	//found it
	if (ptr != tiles.end()) {
		return *ptr;
	}

	//a tileIndex of -1 is an error code, the rest is for show
	return {0,0,0,-1,-1,-1};
}

int Region::DeleteTileR(Tile const& tile) {
	if (!InBoundsR(tile.x, tile.y)) {
		throw(std::runtime_error("Deleted tile location out of bounds"));
	}
	//sentinel/error code
	if (tile.tileIndex == -1) {
		return 0;
	}
	return tiles.erase(tile);
}

bool operator<(Region const& lhs, Region const& rhs) {
	//sort by y -> x
	if (lhs.y == rhs.y) {
		return lhs.x < rhs.x;
	}
	return lhs.y < rhs.y;
}

inline bool operator>(Region const& lhs, Region const& rhs) {
	//wrap the other operator
	return rhs < lhs;
}

inline bool operator==(Region const& lhs, Region const& rhs) {
	//comparisons work on the location ONLY
	//this function is redundant as far as the std::set object is concerned
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}
