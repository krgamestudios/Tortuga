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
#ifndef REGIONPAGER_HPP_
#define REGIONPAGER_HPP_

#include "region.hpp"

#include <list>

class RegionPager {
public:
	RegionPager() = delete;
	RegionPager(int regionWidth, int regionHeight, int regionDepth);
	~RegionPager();

	int SetTile(int x, int y, int z, int v);
	int GetTile(int x, int y, int z);

	Region* GetRegion(int x, int y);

	void LoadRegion(int x, int y);
	void SaveRegion(int x, int y);
	void CreateRegion(int x, int y);
	void UnloadRegion(int x, int y);

	//accessors
	int GetRegionWidth() { return regionWidth; }
	int GetRegionHeight() { return regionHeight; }
	int GetRegionDepth() { return regionDepth; }
private:
	const int regionWidth;
	const int regionHeight;
	const int regionDepth;
	std::list<Region> regionList;
};

#endif
