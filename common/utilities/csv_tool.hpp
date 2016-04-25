/* Copyright: (c) Kayne Ruse 2015
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

//DOCS: this is a generic CSV reading tool
//DOCS: empty lines and comment lines begining with '#' are ignored
//DOCS: whitespace characters are valid field values
//DOCS: if the file is invalid, then the behavior is undefined

#include <array>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//define the container types
template<int N>
using CSVElement = std::array<std::string, N>;

template<int N>
using CSVObject = std::vector<CSVElement<N>>;

//read a file into an object
template<int N>
CSVObject<N> readCSV(std::string fname, char delim = ',') {
	//open the file
	std::ifstream is(fname);

	if (!is.is_open()) {
		std::ostringstream msg;
		msg << "Failed to open file: " << fname;
		throw(std::runtime_error(msg.str()));
	}

	//build the scanf format
	std::ostringstream format;
	format << "%[^\0" << delim << "]";

	//read and store each record (one per line)
	CSVObject<N> object;
	while(!is.eof()) {
		//get a line
		std::string tmpLine;
		getline(is, tmpLine);

		//skip blank and comment lines
		if (tmpLine.size() == 0 || tmpLine[0] == '#') {
			continue;
		}

		//read and store each field
		CSVElement<N> record;
		for (int i = 0; i < N; ++i) {
			//get a field
			char tmpField[256];
			memset(tmpField, 0, 256);

			sscanf(tmpLine.c_str(), format.str().c_str(), tmpField);

			//prune the input
			int len = std::min(strlen(tmpField)+1, tmpLine.size());
			tmpLine = tmpLine.substr(len);

			//store the field
			record[i] = tmpField;
		}

		object.push_back(record);
	}

	//finally, close the file
	is.close();

	return object;
}

template<int N>
void writeCSV(std::string fname, CSVObject<N> const& object, char delim = ',') {
	//open the file
	std::ofstream os(fname);

	if (!os.is_open()) {
		std::ostringstream msg;
		msg << "Failed to open file: " << fname;
		throw(std::runtime_error(msg.str()));
	}

	//write each record, one at a time
	for(auto& record : object) {
		//write each field, one at a time
		for (int i = 0; i < N; i++) {
			os << record[i];

			//print delimiter
			if (i != N -1) {
				os << delim;
			}
		}
		os << std::endl;
	}

	//finish
	os.close();
}
