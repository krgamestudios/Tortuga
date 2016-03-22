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
#pragma once

#include <chrono>

class FrameRate {
public:
	typedef std::chrono::high_resolution_clock Clock;

	FrameRate() = default;

	//DOCS: for usage purposes, this returns -1 unless a new framerate value is set
	int Calculate() {
		frameCount++;
		if (Clock::now() - tick >= std::chrono::duration<int>(1)) {
			lastFrameRate = frameCount;
			frameCount = 0;
			tick = Clock::now();
			return lastFrameRate;
		}
		return -1;
	}

	int GetFrameRate() { return lastFrameRate; }

private:
	int frameCount = 0;
	int lastFrameRate = 0;
	Clock::time_point tick = Clock::now();
};
