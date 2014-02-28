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
#include "region.hpp"

#include <cstdlib>

Region::Region(int argWidth, int argHeight, int argDepth, int argX, int argY):
	width(argWidth),
	height(argHeight),
	depth(argDepth),
	x(argX),
	y(argY)
{
	tiles = new int**[width];
	for (int i = 0; i < width; ++i) {
		tiles[i] = new int*[height];
		for (int j = 0; j < height; ++j) {
			tiles[i][j] = new int[depth];
			for (int k = 0; k < depth; ++k) {
				tiles[i][j][k] = 0;
			}
		}
	}
}

Region::~Region() {
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; j++) {
			delete tiles[i][j];
		}
		delete tiles[i];
	}
	delete tiles;
}

int Region::SetTile(int x, int y, int z, int v) {
	return tiles[x][y][z] = v;
}

int Region::GetTile(int x, int y, int z) {
	return tiles[x][y][z];
}
