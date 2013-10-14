#ifndef MAPLOADER_HPP_
#define MAPLOADER_HPP_

#include "region_pager.hpp"
#include "tile_sheet.hpp"

#include <list>
#include <string>

/* Given the map name and pointers to containers, this is a generic loading system
*/
void loadGameMap(std::string mapPathName, RegionPager* pager, std::list<TileSheet>* sheetList);
void saveGameMap(std::string mapPathName, RegionPager* pager, std::list<TileSheet>* sheetList);

#endif
