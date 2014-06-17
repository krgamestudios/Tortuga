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
#ifndef SIMPLERNG_HPP_
#define SIMPLERNG_HPP_

class SimpleRNG {
public:
	SimpleRNG() = default;
	SimpleRNG(int x) { SetSeed(x); }

	int SetSeed(int s) { return seed = s; }
	int GetSeed() { return seed; }

	int SimpleRNG::operator()(int x) {
		return (x + seed) * 345589144 + 1123581321 + seed;
	};

private:
	int seed = 8891;
};

#endif