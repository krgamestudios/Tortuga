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

#include <stdexcept>
#include <sstream>

static int snap(int base, int x) {
	if (x < 0) {
		x++;
		return x - x % base - base;
	}
	return x - x % base;
}

Region::Region(int i, int j, int k, int l):
	x(i),
	y(j),
	width(k),
	height(l)
{
	//make sure that the region's position lines up
	if (x != snap(width, x) || y != snap(height, y)) {
		std::ostringstream os;
		os << "Region is unaligned; x: " << x << ", y: " << y << ", width: " << width << ", height: " << height;
		throw(std::runtime_error(os.str()));
	}
}

bool Region::NewTile(Tile const& tile) {
	if (!InBounds(tile.x, tile.y)) {
		throw(std::runtime_error("New tile location out of bounds"));
	}

	bool ret = tiles.erase(tile);
	tiles.insert(tile);
	return ret;
}

Tile Region::GetTile(int x, int y, int tw, int th, int minDepth) {
	/* This method is used to find a tile that covers (x, y).
	 * This method returns the first tile above minDepth.
	 * Since Region & Tile do not save the tile's size, tw & th are the size of the tiles.
	*/
	std::set<Tile>::iterator ptr = tiles.begin();
	while(ptr != tiles.end()) {
		//skip the tiles that are too deep
		if (ptr->depth >= minDepth) {
			break;
		}
		ptr++;
	}
	while(ptr != tiles.end()) {
		//find the first tile here
		if ((ptr->x <= x) && (ptr->y <= y) && (ptr->x + tw > x) && (ptr->y + th > y)) {
			break;
		}
		ptr++;
	}
	if (ptr != tiles.end()) {
		return *ptr;
	}
	return {0,0,0,-1};
}

bool Region::DeleteTile(Tile const& tile) {
	if (!InBounds(tile.x, tile.y)) {
		throw(std::runtime_error("Deleted tile location out of bounds"));
	}
	return tiles.erase(tile);
}

bool Region::InBounds(int x, int y) {
	return (x > this->x) && (y > this->y) && (x < this->x + this->width) && (y < this->y + this->height);
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
