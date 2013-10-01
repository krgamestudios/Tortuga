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
#ifndef MAP_HPP_
#define MAP_HPP_

#include "tile.hpp"
#include "region.hpp"
#include "tile_sheet.hpp"

#include <list>
#include <map>
#include <string>

class Map {
public:
	Map() = default;
	~Map() = default;

	//simple call
	void DrawTo(SDL_Surface* const, int camX, int camY);

	//TODO: baking goes here...

	//TODO: prune the map's data

	//manage the whole map
	void NewMap(std::string fname, int regionWidth, int regionHeight);
	void LoadMap(std::string fname);
	void UnloadMap();
	void DeleteMap();

	//for editing
	bool NewTile(Tile const&);
	Tile GetTile(int x, int y, int minDepth);
	void DeleteTile(Tile const&);

	//for pagination, etc.
	Region* NewRegion(int x, int y);
	Region* GetRegion(int x, int y);
	void DeleteRegion(int x, int y);
	void FreeRegion(int x, int y);

	//all tile sheets are loaded into memory during operation
	TileSheet* NewTileSheet(std::string name, Uint16 w, Uint16 h);
	TileSheet* NewTileSheet(SDL_Surface* const, Uint16 w, Uint16 h);
	TileSheet* GetTileSheet(std::string name);
	TileSheet* GetTileSheetByRange(int tileIndex);
	void DeleteTileSheet(std::string name);

	//raw accessors and mutators
	std::string GetName() const { return name; }

	int GetRegionWidth() const { return regionWidth; }
	int GetRegionHeight() const { return regionHeight; }
	int GetTileRangeEnd() const { return tileRangeEnd; }

	std::list<Region>* GetRegions() { return &regions; }
	std::map<int, TileSheet>* GetTileSheets() { return &tileSheets; }

private:
	std::string name; //the name of the map
	int regionWidth = 0, regionHeight = 0; //for pagination
	int tileRangeEnd = 0; //for managing the tile sheet ranges

	std::list<Region> regions;
	std::map<int, TileSheet> tileSheets;
};

#endif
