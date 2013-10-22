#ifndef MAPLOADER_HPP_
#define MAPLOADER_HPP_

#include "region_pager.hpp"
#include "tile_sheet_manager.hpp"

#include <string>

/* Given the map name and pointers to containers, this is a generic loading system
*/
void loadGameMap(std::string mapPathName, RegionPager* const, TileSheetManager* const);
void saveGameMap(std::string mapPathName, RegionPager* const, TileSheetManager* const);

#endif
