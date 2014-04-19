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
	virtual ~RegionPagerBase() = default;

	//tile manipulation
	Region::type_t SetTile(int x, int y, int z, Region::type_t v);
	Region::type_t GetTile(int x, int y, int z);

	//region manipulation
	Region* GetRegion(int x, int y);
	Region* FindRegion(int x, int y);
	Region* PushRegion(Region*);

	//interface
	virtual Region* LoadRegion(int x, int y) = 0;
	virtual Region* SaveRegion(int x, int y) = 0;
	virtual Region* CreateRegion(int x, int y) = 0;
	virtual void UnloadRegion(int x, int y) = 0;
	//TODO: delete?

	//accessors & mutators
	std::list<Region*>* GetContainer() { return &regionList; }
protected:
	std::list<Region*> regionList;
};

template<typename MapGenerator, typename MapFileFormat>
class RegionPager : public RegionPagerBase {
public:
	RegionPager() = default;
	~RegionPager() {
		UnloadAll();
	}

	Region* LoadRegion(int x, int y) {
		//snap the coords
		x = snapToBase(REGION_WIDTH, x);
		y = snapToBase(REGION_HEIGHT, y);

		//load the region if possible
		Region* ptr = nullptr;
		format.Load(&ptr, x, y);
		if (ptr) {
			return PushRegion(ptr);
		}
		return nullptr;
	}

	Region* SaveRegion(int x, int y) {
		//snap the coords
		x = snapToBase(REGION_WIDTH, x);
		y = snapToBase(REGION_HEIGHT, y);

		//find & save the region
		Region* ptr = FindRegion(x, y);
		if (ptr) {
			format.Save(ptr);
		}
		return ptr;
	}

	Region* CreateRegion(int x, int y) {
		//snap the coords
		x = snapToBase(REGION_WIDTH, x);
		y = snapToBase(REGION_HEIGHT, y);

		//create and push the object
		Region* ptr = nullptr;
		generator.Create(&ptr, x, y);
		return PushRegion(ptr);
	}

	void UnloadRegion(int x, int y) {
		//snap the coords
		x = snapToBase(REGION_WIDTH, x);
		y = snapToBase(REGION_HEIGHT, y);

		//custom loop
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
	void UnloadAll() {
		for (auto& it : regionList) {
			generator.Unload(it);
		}
		regionList.clear();
	}

	//accessors
	MapGenerator* GetGenerator() { return &generator; }
	MapFileFormat* GetFormat() { return &format; }
protected:
	MapGenerator generator;
	MapFileFormat format;
};

#endif
