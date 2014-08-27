/* Copyright: (c) Kayne Ruse 2013, 2014
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
#include "utility.hpp"

#include <algorithm>

std::string truncatePath(std::string pathname) {
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

std::string to_string_custom(int i) {
	char buffer[20];
	snprintf(buffer, 20, "%d", i);
	return std::string(buffer);
}

int to_integer_custom(std::string s) {
	int ret = 0;
	sscanf(s.c_str(), "%d", &ret);
	return ret;
}