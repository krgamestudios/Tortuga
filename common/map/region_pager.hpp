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

#include <algorithm>
#include <list>

class RegionPagerBase {
public:
	RegionPagerBase() = delete;
	RegionPagerBase(int regionWidth, int regionHeight, int regionDepth);
	~RegionPagerBase();

	int SetTile(int x, int y, int z, int v);
	int GetTile(int x, int y, int z);

	void Update();

	Region* GetRegion(int x, int y);

	//interface
	virtual Region* LoadRegion(int x, int y) = 0;
	virtual Region* SaveRegion(int x, int y) = 0;
	virtual Region* CreateRegion(int x, int y) = 0;
	virtual void UnloadRegion(int x, int y) = 0;

	//accessors
	int GetRegionWidth() { return regionWidth; }
	int GetRegionHeight() { return regionHeight; }
	int GetRegionDepth() { return regionDepth; }
protected:
	const int regionWidth;
	const int regionHeight;
	const int regionDepth;
	std::list<Region> regionList;
};

template<typename MapGenerator, typename MapFileFormat>
class RegionPager : public RegionPagerBase {
public:
	RegionPager() = delete;
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
		Region* region = nullptr;
		format.Load(&region, x, y);
		if (region) {
			regionList.push_back(std::move(*region));
			return &regionList.back();
		}
		return nullptr;
	}

	Region* SaveRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		//find the specified region
		auto iter = std::find_if(regionList.begin(), regionList.end(), [x, y](Region& it){
			return it.GetX() == x && it.GetY() == y;
		});

		//save the region if it's loaded
		if (iter != regionList.end()) {
			format.Save(&(*iter, x, y));
			return &(*iter);
		}
		return nullptr;
	}

	Region* CreateRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		//create and push the object
		Region* region = nullptr;
		generator.Create(&region);
		regionList.push_back(std::move(*region));
		return regionList.back();
	}

	void UnloadRegion(int x, int y) {
		//snap the coords
		x = snapToBase(regionWidth, x);
		y = snapToBase(regionHeight, y);

		//find the specified region
		auto iter = std::find_if(regionList.begin(), regionList.end(), [x, y](Region& it){
			return it.GetX() == x && it.GetY() == y;
		});

		//pass it to the generator for unloading
		if (iter != regionList.end()) {
			generator.Unload(&(*iter));
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
