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
#include "config_utility.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

void ConfigUtility::Load(std::string fname, bool skipMissingFile, int argc, char* argv[]) {
	//clear the stored configuration
	configMap.clear();

	//use the default file
	if (argc < 2) {
		configMap = Read(fname, skipMissingFile);
		return;
	}

	//some variables to use
	table_t redirectedFile;
	table_t cmdLineParams;
	char key[256], val[256];
	bool redirectUsed = false;

	//reading from the command line
	for (int i = 1; i < argc; ++i) {
		//read from a specified config file
		if (!strncmp(argv[i], "-config=", 8)) {
			//older specified files take precedence
			table_t tmp = Read(argv[i] + 8, skipMissingFile);
			redirectedFile.insert(tmp.begin(), tmp.end());
			redirectUsed = true;
			continue;
		}

		//set some specific values
		if (!strncmp(argv[i], "-", 1)) {
			//wipe the variables
			memset(key, 0, 256);
			memset(key, 0, 256);

			//read the key-value pair
			if (sscanf(argv[i], "-%[^=]=%[^\0]", key, val) != 2) {
				std::ostringstream os;
				os << "Failed to read a command line config argument (expected -%s=%s):" << std::endl;
				os << "\targv[" << i << "]: " << argv[i] << std::endl;
				os << "\tkey: " << key << std::endl;
				os << "\tval: " << val << std::endl;
				throw(std::runtime_error( os.str() ));
			}
			cmdLineParams[key] = val;
		}
	}

	//finally, construct the final config table
	if (!redirectUsed) {
		redirectedFile = Read(fname, skipMissingFile);
	}
	configMap.insert(cmdLineParams.begin(), cmdLineParams.end());
	configMap.insert(redirectedFile.begin(), redirectedFile.end());
}

ConfigUtility::table_t ConfigUtility::Read(std::string fname, bool skipMissingFile) {
	//read in and return this file's data
	table_t retTable;
	std::ifstream is(fname);

	if (!is.is_open()) {
		if (skipMissingFile) {
			return {}; //empty table
		}
		std::ostringstream os;
		os << "Failed to open a config file: " << fname;
		throw(std::runtime_error( os.str() ));
	}

	std::string key, val;

	while(true) { //forever
		//eat whitespace
		while(isspace(is.peek())) {
			is.ignore();
		}

		//end of file
		if (is.eof()) {
			break;
		}

		//skip comment lines
		if (is.peek() == '#') {
			while(is.peek() != '\n' && !is.eof()) {
				is.ignore();
			}
			continue;
		}

		//read in the pair
		getline(is, key,'=');
		getline(is, val);

		//eat the whitespace at the start & end
		while(key.size() && isspace( *key.begin() )) {
			key.erase(0, 1);
		}
		while(val.size() && isspace( *val.begin() )) {
			val.erase(0, 1);
		}

		while(key.size() && isspace( *(key.end()-1) )) {
			key.erase(key.end() - 1);
		}
		while(val.size() && isspace( *(val.end()-1) )) {
			val.erase(val.end() - 1);
		}

		//disallow empty/wiped pairs
		if (key.size() == 0 || val.size() == 0) {
			continue;
		}

		//save the pair
		retTable[key] = val;
	}

	is.close();

	//load in any subordinate config files
	if (retTable.find("config.next") != retTable.end()) {
		table_t subTable = Read(retTable["config.next"], skipMissingFile);
		retTable.insert(subTable.begin(), subTable.end());
	}

	return retTable;
}

//-------------------------
//Convert to a type
//-------------------------

std::string& ConfigUtility::String(std::string s) {
	return configMap[s];
}

int ConfigUtility::Integer(std::string s) {
	table_t::iterator it = configMap.find(s);
	if (it == configMap.end()) {
		return 0;
	}
	return atoi(it->second.c_str());
}

double ConfigUtility::Double(std::string s) {
	table_t::iterator it = configMap.find(s);
	if (it == configMap.end()) {
		return 0.0;
	}
	return atof(it->second.c_str());
}

bool ConfigUtility::Boolean(std::string s) {
	table_t::iterator it = configMap.find(s);
	if (it == configMap.end()) {
		return false;
	}
	return it->second == "true";
}
