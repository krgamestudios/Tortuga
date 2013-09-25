#include "tile_sheet.hpp"

#include <algorithm>
#include <stdexcept>

static inline std::string truncatePath(std::string pathname) {
	//TODO: move this into a utilities module?
	return std::string(
		std::find_if(
			pathname.rbegin(),
			pathname.rend(),
			[](char ch) -> bool {
				//windows only
				return ch == '/' || ch == '\\';
//				//unix only
//				return ch == '/';
			}).base(),
		pathname.end());
}

SDL_Surface* TileSheet::LoadSurface(std::string fname, Uint16 w, Uint16 h) {
	//setup the image
	image.LoadSurface(fname);
	image.SetClipW(w);
	image.SetClipH(h);

	//get the tile counts
	xCount = image.GetSurface()->w / w;
	yCount = image.GetSurface()->h / h;
	totalCount = xCount * yCount;

	//cache the name
	name = truncatePath(fname);
}

SDL_Surface* TileSheet::SetSurface(SDL_Surface* const ptr, Uint16 w, Uint16 h) {
	//setup the image
	image.SetSurface(ptr);
	image.SetClipW(w);
	image.SetClipH(h);

	//get the tile counts
	xCount = image.GetSurface()->w / w;
	yCount = image.GetSurface()->h / h;
	totalCount = xCount * yCount;
}

SDL_Surface* TileSheet::GetSurface() {
	return image.GetSurface();
}

void TileSheet::FreeSurface() {
	image.FreeSurface();
	totalCount = xCount = yCount = 0;
}

void TileSheet::DrawTo(SDL_Surface* const dest, int x, int y, int tileIndex) {
	if (!InRange(tileIndex)) {
		throw(std::runtime_error("Tile index out of range of Tile Sheet"));
	}
	Sint16 clipX = tileIndex % xCount * image.GetClipW();
	Sint16 clipY = tileIndex / xCount * image.GetClipH();

	image.SetClipX(clipX);
	image.SetClipY(clipY);

	image.DrawTo(dest, x, y);
}
