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
#ifndef ACCOUNTDATA_HPP_
#define ACCOUNTDATA_HPP_

#include <string>

class AccountData {
public:
	AccountData() = default;
	~AccountData() = default;

	//accessors and mutators
	int SetClientIndex(int i);
	int GetClientIndex();

	std::string SetUsername(std::string s);
	std::string GetUsername();

	//database stuff
	bool GetBlackListed();
	bool GetWhiteListed();
	bool GetModerator();
	bool GetAdministrator();

private:
	friend class AccountManager;

	int clientIndex;
	std::string username;
	//password/auth token

	//bit fields?
	bool blackListed = false;
	bool whiteListed = true;
	bool mod = false;
	bool admin = false;
};

#endif
