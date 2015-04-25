/* Copyright: (c) Kayne Ruse 2013-2015
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
#include "region_checksum.hpp"

int debugRegionSum(Region* const region) {
	int sum = 0;
	for(int i = 0; i < REGION_WIDTH; i++) {
		for (int j = 0; j < REGION_HEIGHT; j++) {
			for (int k = 0; k < REGION_DEPTH; k++) {
				sum += region->GetTile(i, j, k);
			}
		}
	}
	return sum;
}