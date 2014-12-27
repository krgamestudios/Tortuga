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
#ifndef CONFIGUTILITY_HPP_
#define CONFIGUTILITY_HPP_

#include "singleton.hpp"

#include <map>
#include <string>

class ConfigUtility : public Singleton<ConfigUtility> {
public:
	void Load(std::string fname, int argc = 0, char* argv[] = nullptr);

	//convert to a type
	std::string& String(std::string);
	int Integer(std::string);
	double Double(std::string);
	bool Boolean(std::string);

	//shorthand
	inline std::string& operator[](std::string s) { return configMap[s]; }
	inline int Int(std::string s) { return Integer(s); }
	inline bool Bool(std::string s) { return Boolean(s); }

private:
	typedef std::map<std::string, std::string> table_t;

	friend Singleton<ConfigUtility>;

	ConfigUtility() = default;
	~ConfigUtility() = default;

	table_t Read(std::string fname);

	table_t configMap;
};

#endif
