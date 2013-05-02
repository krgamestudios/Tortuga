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
	std::map<std::string, std::string>* GetMap() {
		return &table;
	}
private:
	std::map<std::string, std::string> table;
};

#endif
