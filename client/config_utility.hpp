/* Copyright: (c) Kayne Ruse 2013
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
#ifndef CONFIGUTILITY_HPP_
#define CONFIGUTILITY_HPP_

#include <map>
#include <string>
#include <cstdlib>

class ConfigUtility {
public:
	ConfigUtility() {}
	ConfigUtility(std::string s) { Load(s); }

	void Load(std::string fname);

	std::string String(std::string s) {
		return table[s];
	}
	const char* CString(std::string s) {
		return table[s].c_str();
	}
	int Integer(std::string s) {
		return atoi(table[s].c_str());
	}
	double Double(std::string s) {
		return atof(table[s].c_str());
	}
	bool Boolean(std::string s) {
		return table[s] == "true";
	}

	std::string& operator[](std::string s) {
		return table[s];
	}
	int Int(std::string s) {
		return Integer(s);
	}
	int Bool(std::string s) {
		return Boolean(s);
	}
	std::map<std::string, std::string>* GetMap() {
		return &table;
	}
private:
	std::map<std::string, std::string> table;
};

#endif
