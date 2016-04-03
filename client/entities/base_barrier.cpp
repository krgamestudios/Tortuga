/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "base_barrier.hpp"

#include "config_utility.hpp"

#include <cstring>

void BaseBarrier::CorrectSprite() {
	//TODO: (9) BaseBarrier::CorrectSprite()
}

int BaseBarrier::SetStatus(int k, int v) {
	if (k >= 8 || k < 0) {
		return -1;
	}
	return status[k] = v;
}

int BaseBarrier::FindStatus(int k) {
	if (k >= 8 || k < 0) {
		return -1;
	}
	return status[k];
}

int* BaseBarrier::SetStatusArray(int* ptr) {
	memcpy(status, ptr, sizeof(int) * 8);
	return status;
}

int* BaseBarrier::GetStatusArray() {
	return status;
}
