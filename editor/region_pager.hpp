#ifndef REGIONPAGER_HPP_
#define REGIONPAGER_HPP_

#include "region.hpp"

#include <list>

class RegionPager {
public:
	RegionPager();
	~RegionPager();

	//these parameters MUST be multiples of the width & height
	Region* NewRegion(int x, int y);
	Region* GetRegion(int x, int y);
	void DeleteRegion(int x, int y);

	void SetOnDelete(void(*f)(Region* const)) { onDelete = f; }
	void SetOnNew(void(*f)(Region* const)) { onNew = f; }

	//accessors and mutators
	int SetWidth(int i) { return regionWidth = i; }
	int SetHeight(int i) { return regionHeight = i; }

	int GetWidth() const { return regionWidth; }
	int GetHeight() const { return regionHeight; }

	std::list<Region>* GetRegions() { return &regionList; }
private:
	std::list<Region> regionList;
	int regionWidth = 0, regionHeight = 0;
	void (*onDelete)(Region* const) = nullptr;
	void (*onNew)(Region* const) = nullptr;
};

#endif
