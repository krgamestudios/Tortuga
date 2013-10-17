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
#ifndef REGIONPAGER_HPP_
#define REGIONPAGER_HPP_

#include "region.hpp"
#include "tile_sheet.hpp"

#include <list>

class RegionPager {
public:
	//for simplicity and consistency
	typedef void (*regionCallback_t)(Region* const);

	RegionPager();
	~RegionPager();

	//these parameters MUST be multiples of the width & height
	Region* NewRegion(int x, int y);
	Region* GetRegion(int x, int y);
	void DeleteRegion(int x, int y);

	//call this to draw to the screen
	void DrawTo(SDL_Surface* const, std::list<TileSheet>* const, int camX, int camY);

	//callback hooks
	void SetOnNew(regionCallback_t f) { onNew = f; }
	void SetOnDelete(regionCallback_t f) { onDelete = f; }

	//params: Absolute values
	void Prune(int left, int top, int right, int bottom);

	//accessors and mutators
	int SetWidth(int i) { return regionWidth = i; }
	int SetHeight(int i) { return regionHeight = i; }

	int GetWidth() const { return regionWidth; }
	int GetHeight() const { return regionHeight; }

	std::list<Region>* GetRegions() { return &regionList; }
private:
	std::list<Region> regionList;
	int regionWidth = 0, regionHeight = 0;

	regionCallback_t onNew = nullptr;
	regionCallback_t onDelete = nullptr;
};

#endif
