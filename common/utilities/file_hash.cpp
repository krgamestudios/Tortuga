/* Copyright: (c) Kayne Ruse 2016
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
#include "file_hash.hpp"

#include <fstream>

//hash a byte array into a 32-bit integer
unsigned fnv_hash_1a_32(void *key, int len) {
	unsigned char *p = static_cast<unsigned char*>(key);
	unsigned h = 0x811c9dc5;
	for (int i = 0; i < len; i++) {
		h = ( h ^ p[i] ) * 0x01000193;
	}
	return h;
}

int getFileHash(std::string fname) {
	std::ifstream is(fname, std::ios::in | std::ios::binary);

	//if the file doesn't exist, return a hash of -1
	if (!is.is_open()) {
		return -1;
	}

	//get the file size
	is.seekg(0, std::ios_base::end);
	int size = is.tellg();
	is.seekg(0);

	//create a buffer of that size
	char buffer[size];

	//load the data
	is.read(buffer, size);

	//cleanup
	is.close();

	//finally, return the hash value
	return fnv_hash_1a_32(buffer, size);
}
