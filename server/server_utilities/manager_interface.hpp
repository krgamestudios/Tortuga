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
#ifndef MANAGERINTERFACE_HPP_
#define MANAGERINTERFACE_HPP_

#include <functional>
#include <map>

template<typename T, typename... Arguments>
class ManagerInterface {
public:
	//common public methods
	virtual int Create(Arguments... parameters) = 0;
	virtual int Load(Arguments... parameters) = 0;
	virtual int Save(int uid) = 0;
	virtual void Unload(int uid) = 0;
	virtual void Delete(int uid) = 0;

	virtual void UnloadAll() = 0;
	virtual void UnloadIf(std::function<bool(std::pair<const int, T>&)> fn) = 0;

	//accessors & mutators
	virtual T* Get(int uid) = 0;
	virtual int GetLoadedCount() = 0;
	virtual int GetTotalCount() = 0; //can be an alias of GetLoadedCount()
	virtual std::map<int, T>* GetContainer() = 0;

protected:
	ManagerInterface() = default;
	~ManagerInterface() = default;

	//members
	std::map<int, T> elementMap;
};

#endif