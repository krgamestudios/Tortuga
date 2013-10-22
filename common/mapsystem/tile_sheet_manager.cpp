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
#include "tile_sheet_manager.hpp"

#include "utility.hpp"

#include <stdexcept>

TileSheet* TileSheetManager::LoadSheet(std::string fname, Uint16 w, Uint16 h) {
	//get the key
	std::string key = truncatePath(fname);

	//don't override what's already here
	if (sheetMap.find(key) != sheetMap.end()) {
		throw(std::runtime_error("Cannot load duplicate tile sheets"));
	}

	//load & setup the sheet object
	sheetMap[key].Load(fname, w, h);
	sheetMap[key].SetBegin(rangeEnd);
	rangeEnd += sheetMap[key].GetTotalCount();
	sheetMap[key].SetEnd(rangeEnd);

	//you can modify the object, say, during the save & load routines...
	return &sheetMap[key];
}

TileSheet* TileSheetManager::GetSheet(std::string name) {
	return &sheetMap.at(name);
}

TileSheet* TileSheetManager::GetSheetByIndex(int tileIndex) {
	for (auto& it : sheetMap) {
		if (it.second.InRange(tileIndex)) {
			return &it.second;
		}
	}
	return nullptr;
}

void TileSheetManager::UnloadSheet(std::string name) {
	sheetMap.erase(name);
}

void TileSheetManager::DrawTo(SDL_Surface* const dest, int x, int y, int tileIndex) {
	for (auto& it : sheetMap) {
		if (it.second.InRange(tileIndex)) {
			it.second.DrawTo(dest, x, y, tileIndex);
			return;
		}
	}
	//No matching tile index
	throw(std::invalid_argument("Tile index is out of range of all tile sheets"));
}
