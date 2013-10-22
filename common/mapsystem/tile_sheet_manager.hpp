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
#ifndef TILESHEETMANAGER_HPP_
#define TILESHEETMANAGER_HPP_

#include "tile_sheet.hpp"

#include <map>
#include <string>

class TileSheetManager {
public:
	TileSheetManager() = default;
	~TileSheetManager() = default;

	TileSheet* LoadSheet(std::string fname, Uint16 w, Uint16 h);
	TileSheet* GetSheet(std::string name);
	TileSheet* GetSheetByIndex(int tileIndex);
	void UnloadSheet(std::string name);

	void DrawTo(SDL_Surface* const, int x, int y, int tileIndex);

	int SetRangeEnd(int i) { return rangeEnd += i; }
	int GetRangeEnd() const { return rangeEnd; }

	std::map<std::string, TileSheet>* GetSheetMap() { return &sheetMap; }
private:
	std::map<std::string, TileSheet> sheetMap;
	int rangeEnd = 0;
};

#endif
