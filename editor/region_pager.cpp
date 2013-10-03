#include "region_pager.hpp"

#include <stdexcept>

RegionPager::RegionPager() {
	//
}

RegionPager::~RegionPager() {
	if (onDelete) {
		for (auto& i : regionList) {
			onDelete(&i);
		}
	}
}

Region* RegionPager::NewRegion(int x, int y) {
	for (auto& i : regionList) {
		if (i.GetX() == x && i.GetY() == y) {
			throw(std::runtime_error("Duplicate Regions detected"));
		}
	}

	regionList.push_front({x, y, regionWidth, regionHeight});
	if (onNew) {
		onNew(&regionList.front());
	}
	return &regionList.front();
}

Region* RegionPager::GetRegion(int x, int y) {
	for (auto& i : regionList) {
		if (i.GetX() == x && i.GetY() == y) {
			return &i;
		}
	}
	//create, insert and return
	regionList.push_front({x, y, regionWidth, regionHeight});
	if (onNew) {
		onNew(&regionList.front());
	}
	return &regionList.front();
}

void RegionPager::DeleteRegion(int x, int y) {
	for (std::list<Region>::iterator i = regionList.begin(); i != regionList.end(); i++) {
		if (i->GetX() == x && i->GetY() == y) {
			if (onDelete) {
				onDelete(&(*i));
			}
			regionList.erase(i);
			break;
		}
	}
}
