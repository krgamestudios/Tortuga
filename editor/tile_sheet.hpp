#ifndef TILESHEET_HPP_
#define TILESHEET_HPP_

#include "image.hpp"

#include <string>

/* The TileSheet class is used for drawing tiles of the map to the screen.
 * This class also has some code to allow the management of several TileSheets,
 * namely the index, begin & end members, tracking of the tile indexes, and
 * custom wrapper functions.
*/
class TileSheet {
public:
	TileSheet() = default;
	~TileSheet() = default;

	//these load/set functions need to be followed by bookkeeping code
	//w & h are the width & height of individual tiles
	SDL_Surface* LoadSurface(std::string fname, Uint16 w, Uint16 h);
	SDL_Surface* SetSurface(SDL_Surface* const, Uint16 w, Uint16 h);
	SDL_Surface* GetSurface();
	void FreeSurface();

	void DrawTo(SDL_Surface* const dest, int x, int y, int tileIndex);

	bool InRange(int i) { return i >= begin && i < end; }

	//accessors and mutators
	Image* GetImage() { return &image; }

	std::string SetName(std::string s) { return name = s; }
	std::string GetName() const { return name; }

	int GetTotalCount() const { return totalCount; }
	int GetXCount() const { return xCount; }
	int GetYCount() const { return yCount; }

	int SetIndex(int i) { return index = i; }
	int SetBegin(int i) { return begin = i; }
	int SetEnd(int i) { return end = i; }

	int GetIndex() const { return index; }
	int GetBegin() const { return begin; }
	int GetEnd() const { return end; }
private:
	Image image;
	std::string name;

	//these are generated and used by internal processes
	int totalCount = 0, xCount = 0, yCount = 0;
	int index = -1, begin = -1, end = -1;
};

#endif
