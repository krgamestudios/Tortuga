#include "map_loader.hpp"

#include "utility.hpp"

#include <fstream>
#include <stdexcept>

void loadGameMap(std::string mapPathName, RegionPager* const, TileSheetManager* const) {
/*	//open the index file
	std::ifstream indexFile(mapPathName + "\\index");
	if (!indexFile.is_open()) {
		throw(std::runtime_error(std::string("Failed to open game map: ") + mapPathName));
	}

	//read each part of the metadata header
	std::string buffer, knownName;
	int sheetCount = -1, rangeEnd = -1, regionWidth = -1, regionHeight = -1;

	getline(indexFile, knownName);

	if (knownName != truncatePath(mapPathName)) {
		//probably not needed, I'll losen this down the road
		throw(std::runtime_error("Internal and external map names do not match"));
	}

	getline(indexFile, buffer, ',');
	sheetCount = to_integer_custom(buffer);

	getline(indexFile, buffer, ',');
	rangeEnd = to_integer_custom(buffer);

	getline(indexFile, buffer, ',');
	regionWidth = to_integer_custom(buffer);

	getline(indexFile, buffer);
	regionHeight = to_integer_custom(buffer);

	//setup the pager
	pager->GetRegions()->clear();
	pager->SetWidth(regionWidth);
	pager->SetHeight(regionHeight);
	pager->SetOnNew([](Region* const ptr) {
		//TODO
	});
	pager->SetOnDelete([](Region* const ptr) {
		//TODO
	});

	//load all of the tile sheets
	sheetList->clear();
	for (int i = 0; i < sheetCount; i++) {
		sheetList->push_back(TileSheet());

		//get the name, width & height
		std::string sheetName;
		getline(indexFile, sheetName, ',');
		getline(indexFile, buffer, ',');
		int w = to_integer_custom(buffer);
		getline(indexFile, buffer, ',');
		int h = to_integer_custom(buffer);

		//load
		sheetList->back().LoadSurface(std::string("rsc\\graphics\\tilesets\\") + sheetName, w, h);

		//set the range
		getline(indexFile, buffer, ',');
		sheetList->back().SetBegin(to_integer_custom(buffer));
		getline(indexFile, buffer);
		sheetList->back().SetEnd(to_integer_custom(buffer));
	}
	TileSheet::SetRangeEnd(rangeEnd);

	//clean up
	indexFile.close();
*/
}

void saveGameMap(std::string mapPathName, RegionPager* const, TileSheetManager* const) {
/*	//open the index file
	std::ofstream indexFile(mapPathName + "\\index");
	if (!indexFile.is_open()) {
		throw(std::runtime_error(std::string("Failed to open game map: ") + mapPathName));
	}

	//write each part of the metadata header
	indexFile << truncatePath(mapPathName) << std::endl;
	indexFile << sheetList->size() << ", ";
	indexFile << TileSheet::GetRangeEnd() << ", ";
	indexFile << pager->GetWidth() << ", ";
	indexFile << pager->GetHeight() << std::endl;

	//write each tile sheet's data
	for (auto& it : *sheetList) {
		indexFile << it.GetName() << ", ";
		indexFile << it.GetImage()->GetClipW() << ", ";
		indexFile << it.GetImage()->GetClipH() << ", ";
		indexFile << it.GetBegin() << ", ";
		indexFile << it.GetEnd() << std::endl;
	}

	indexFile.close();
*/
}