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
#ifndef DOORMANAGER_HPP_
#define DOORMANAGER_HPP_

#include "door_data.hpp"
#include "manager_interface.hpp"
#include "singleton.hpp"
#include "vector2.hpp"

#include <functional>
#include <string>

class DoorManager:
	public Singleton<DoorManager>,
	public ManagerInterface<DoorData, std::string, Vector2>
{
public:
	//common public methods
	int Create(std::string, Vector2) override;
	int Load(std::string, Vector2) override;
	int Save(int uid) override;
	void Unload(int uid) override;
	void Delete(int uid) override;

	void UnloadAll() override;
	void UnloadIf(std::function<bool(std::pair<const int, DoorData>)> fn) override;

	//accessors & mutators
	DoorData* Get(int uid) override;
	int GetLoadedCount() override;
	int GetTotalCount() override;
	std::map<int, DoorData>* GetContainer() override;

private:
	friend Singleton<DoorManager>;

	DoorManager() = default;
	~DoorManager() = default;
};

#endif