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
#include "utility.hpp"

#include <list>

class RegionPagerBase {
public:
	RegionPagerBase() = default;
	RegionPagerBase(int regionWidth, int regionHeight, int regionDepth);
	virtual ~RegionPagerBase();

	int SetTile(int x, int y, int z, int v);
	int GetTile(int x, int y, int z);

	Region* GetRegion(int x, int y);

	//interface
	virtual Region* LoadRegion(int x, int y) = 0;
	virtual Region* SaveRegion(int x, int y) = 0;
	virtual Region* CreateRegion(int x, int y) = 0;
	virtual void UnloadRegion(int x, int y) = 0;

	//accessors
	//NOTE: don't change the sizes mid-program, it will cause issues
	int SetRegionWidth(int i) { return regionWidth = i; }
	int SetRegionHeight(int i) { return regionHeight = i; }
	int SetRegionDepth(int i) { return regionDepth = i; }

	int GetRegionWidth() const { return regionWidth; }
	int GetRegionHeight() const { return regionHeight; }
	int GetRegionDepth() const { return regionDepth; }
protected:
	int regionWidth;
	int regionHeight;
	int regionDepth;
	std::list<Region*> regionList;
};

template<typename MapGenerator, typename MapFileFormat>
class RegionPager : public RegionPagerBase {
public:
	RegionPager() = default;
	RegionPager(int w, int h, int d):
		RegionPagerBase(w, h, d)
	{
		//EMPTY
	}
	~RegionPager() = default;

	Region* LoadRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		//load the region if possible
		Region* ptr = nullptr;
		format.Load(&ptr, x, y);
		if (ptr) {
			regionList.push_back(ptr);
			return ptr;
		}
		return nullptr;
	}

	Region* SaveRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		//find & save the region
		for (std::list<Region*>::iterator it = regionList.begin(); it != regionList.end(); it++) {
			if ((*it)->GetX() == x && (*it)->GetY() == y) {
				format.Save(*it);
				return *it;
			}
		}
		return nullptr;
	}

	Region* CreateRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		//create and push the object
		Region* ptr = nullptr;
		generator.Create(&ptr, regionWidth, regionHeight, regionDepth, x, y);
		regionList.push_back(ptr);
		return ptr;
	}

	void UnloadRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		for (std::list<Region*>::iterator it = regionList.begin(); it != regionList.end(); /* EMPTY */) {
			if ((*it)->GetX() == x && (*it)->GetY() == y) {
				generator.Unload(*it);
				regionList.erase(it);

				//reset the loop, because of reasons
				it = regionList.begin();
				continue;
			}
			++it;
		}
	}

	//accessors
	MapGenerator* GetGenerator() { return &generator; }
	MapFileFormat* GetFormat() { return &format; }
protected:
	MapGenerator generator;
	MapFileFormat format;
};

#endif
