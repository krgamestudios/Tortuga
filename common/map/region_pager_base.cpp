/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "region_pager_base.hpp"

#include <stdexcept>
#include <algorithm>

RegionPagerBase::~RegionPagerBase() {
	UnloadAll();
};

Region::type_t RegionPagerBase::SetTile(int x, int y, int z, Region::type_t v) {
	Region* ptr = GetRegion(x, y);
	return ptr->SetTile(x - ptr->GetX(), y - ptr->GetY(), z, v);
}

//Bug Origin?
Region::type_t RegionPagerBase::GetTile(int x, int y, int z) {
	Region* ptr = GetRegion(x, y);
	return ptr->GetTile(x - ptr->GetX(), y - ptr->GetY(), z);
}

bool RegionPagerBase::SetSolid(int x, int y, int b) {
	Region* ptr = GetRegion(x, y);
	return ptr->SetSolid(x - ptr->GetX(), y - ptr->GetY(), b);
}

bool RegionPagerBase::GetSolid(int x, int y) {
	Region* ptr = GetRegion(x, y);
	return ptr->GetSolid(x - ptr->GetX(), y - ptr->GetY());
}

Region* RegionPagerBase::GetRegion(int x, int y) {
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//get the region by various means
	Region* ptr = nullptr;
	ptr = FindRegion(x, y);
	if (ptr) return ptr;
	ptr = LoadRegion(x, y);
	if (ptr) return ptr;
	return CreateRegion(x, y);
}

Region* RegionPagerBase::FindRegion(int x, int y) {
	//find the region
	std::list<Region>::iterator it = find_if(regionList.begin(), regionList.end(), [x, y](Region& region) -> bool {
		return region.GetX() == x && region.GetY() == y;
	});
	return it != regionList.end() ? &(*it) : nullptr;
}

Region* RegionPagerBase::PushRegion(Region* const ptr) {
	regionList.push_front(*ptr);
	return &regionList.front();
}

Region* RegionPagerBase::LoadRegion(int x, int y) {
	//EMPTY, intended for override
	return nullptr;
}

Region* RegionPagerBase::SaveRegion(int x, int y) {
	//EMPTY, intended for override
	return nullptr;
}

Region* RegionPagerBase::CreateRegion(int x, int y) {
	if (FindRegion(x, y)) {
		throw(std::logic_error("Cannot overwrite an existing region"));
	}
	regionList.emplace_front(x, y);
	return &regionList.front();
}

void RegionPagerBase::UnloadIf(std::function<bool(Region const&)> fn) {
	regionList.remove_if(fn);
}

void RegionPagerBase::UnloadAll() {
	regionList.clear();
}

std::list<Region>* RegionPagerBase::GetContainer() {
	return &regionList;
}