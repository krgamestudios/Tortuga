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
#ifndef REGIONPAGERBASE_HPP_
#define REGIONPAGERBASE_HPP_

#include "region.hpp"

#include <list>

class RegionPagerBase {
public:
	RegionPagerBase() = default;
	virtual ~RegionPagerBase() { UnloadAll(); };

	//tile manipulation
	virtual Region::type_t SetTile(int x, int y, int z, Region::type_t v);
	virtual Region::type_t GetTile(int x, int y, int z);

	//region manipulation
	virtual Region* GetRegion(int x, int y);
	virtual Region* FindRegion(int x, int y);
	virtual Region* PushRegion(Region* const);

	virtual Region* LoadRegion(int x, int y);
	virtual Region* SaveRegion(int x, int y);
	virtual Region* CreateRegion(int x, int y);
	virtual void UnloadRegion(int x, int y);

	virtual void UnloadAll();

	//accessors & mutators
	std::list<Region>* GetContainer() { return &regionList; }
protected:
	std::list<Region> regionList;
};

#endif
