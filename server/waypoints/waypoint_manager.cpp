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
#include "waypoint_manager.hpp"

int WaypointManager::Create() {
	//TODO
}

void WaypointManager::Unload(int uid) {
	//TODO
}

void WaypointManager::UnloadAll() {
	//TODO
}

void WaypointManager::UnloadIf(std::function<bool(std::pair<const int, WaypointData const&>)> fn) {
	//TODO
}

WaypointData* WaypointManager::Get(int uid) {
	//TODO
}

int WaypointManager::GetLoadedCount() {
	//TODO
}

int WaypointManager::GetTotalCount() {
	//TODO
}

std::map<int, WaypointData>* WaypointManager::GetContainer() {
	//TODO
}
