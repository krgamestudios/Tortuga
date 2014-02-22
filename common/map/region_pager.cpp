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
#include "region_pager.hpp"

#include "utility.hpp"

#include <algorithm>

RegionPagerBase::RegionPagerBase(int argWidth, int argHeight, int argDepth):
	regionWidth(argWidth),
	regionHeight(argHeight),
	regionDepth(argDepth)
{
	//EMPTY
}

RegionPagerBase::~RegionPagerBase() {
	//EMPTY
}

int RegionPagerBase::SetTile(int x, int y, int z, int v) {
	Region* ptr = GetRegion(x, y);
	return ptr->SetTile(x - ptr->GetX(), y = ptr->GetY(), z, v);
}

int RegionPagerBase::GetTile(int x, int y, int z) {
	Region* ptr = GetRegion(x, y);
	return ptr->GetTile(x - ptr->GetX(), y = ptr->GetY(), z);
}

Region* RegionPagerBase::GetRegion(int x, int y) {
	//snap the coords
	x = snapToBase(regionWidth, x);
	y = snapToBase(regionHeight, y);

	Region* ptr = nullptr;

	//find the loaded region
	auto iter = std::find_if(regionList.begin(), regionList.end(), [x,y](Region& it) {
		return it.GetX() == x && it.GetY() == y;
	});
	if (iter != regionList.end()) { //ugly hack
		ptr = &(*iter);
	}

	//or load the region
	if (!ptr) {
		ptr = LoadRegion(x, y);
	}

	//or create the region
	if (!ptr) {
		ptr = CreateRegion(x, y);
	}

	return ptr;
}

void RegionPagerBase::Update() {
	//TODO
}